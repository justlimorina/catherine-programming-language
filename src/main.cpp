#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <csignal>
#include <cstdlib>

#ifdef _WIN32
#include <io.h>
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// Returns true when the accumulated input looks complete (balanced braces,
// parens, brackets, and no unterminated string). Used by the REPL to decide
// when a multi-line block (if/while/struct/func) can be run.
static bool isCompleteInput(const std::string& source) {
    bool inString = false;
    int opens = 0, closes = 0;
    for (char c : source) {
        if (inString) {
            if (c == '"') inString = false;
            continue;
        }
        switch (c) {
            case '"': inString = true; break;
            case '{': case '(': case '[': opens++; break;
            case '}': case ')': case ']': closes++; break;
        }
    }
    return opens <= closes && !inString;
}

static void runSource(Interpreter& interpreter, const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::shared_ptr<StmtAST>> statements = parser.parse();
    interpreter.interpret(statements);
}

// ---- Minimal terminal line editor (readline-lite, cross-platform) ----

#ifdef _WIN32
static DWORD g_originalConsoleModeOut = 0;
static DWORD g_originalConsoleModeIn = 0;
static bool g_consoleModeSaved = false;

static void restoreTermios() {
    if (g_consoleModeSaved) {
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleMode(hIn, g_originalConsoleModeIn);
        SetConsoleMode(hOut, g_originalConsoleModeOut);
    }
}

static void setRawMode(bool enable) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (enable) {
        if (!g_consoleModeSaved) {
            GetConsoleMode(hIn, &g_originalConsoleModeIn);
            GetConsoleMode(hOut, &g_originalConsoleModeOut);
            g_consoleModeSaved = true;
        }
        DWORD inMode = g_originalConsoleModeIn & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hIn, inMode);
        DWORD outMode = g_originalConsoleModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, outMode);
    } else {
        restoreTermios();
    }
}

static bool isTerminal() {
    return _isatty(_fileno(stdin)) != 0;
}
#else
static termios g_originalTermios;
static bool g_termiosSaved = false;

static void restoreTermios() {
    if (g_termiosSaved) tcsetattr(STDIN_FILENO, TCSANOW, &g_originalTermios);
}

static void setRawMode(bool enable) {
    if (enable) {
        termios raw = g_originalTermios;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    } else {
        restoreTermios();
    }
}

static bool isTerminal() {
    return isatty(STDIN_FILENO) != 0;
}
#endif

enum KeyType {
    KEY_NONE = 0,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_CTRL_D,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_CHAR,
    KEY_EOF
};

struct InputKey {
    KeyType type;
    char ch;
};

static InputKey getNextKey() {
#ifdef _WIN32
    int c = _getch();
    if (c == EOF || c == 26 /* Ctrl-Z */) {
        return { KEY_EOF, 0 };
    }
    if (c == 4 /* Ctrl-D */) {
        return { KEY_CTRL_D, 0 };
    }
    if (c == 13 || c == 10) {
        return { KEY_ENTER, 0 };
    }
    if (c == 8 || c == 127) {
        return { KEY_BACKSPACE, 0 };
    }
    if (c == 0 || c == 224) {
        int c2 = _getch();
        if (c2 == 72) return { KEY_UP, 0 };
        if (c2 == 80) return { KEY_DOWN, 0 };
        if (c2 == 77) return { KEY_RIGHT, 0 };
        if (c2 == 75) return { KEY_LEFT, 0 };
        return { KEY_NONE, 0 };
    }
    if (c >= 32 && c <= 126) {
        return { KEY_CHAR, static_cast<char>(c) };
    }
    return { KEY_NONE, 0 };
#else
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return { KEY_EOF, 0 };
    }
    if (c == '\n' || c == '\r') {
        return { KEY_ENTER, 0 };
    }
    if (c == 127 || c == 8) {
        return { KEY_BACKSPACE, 0 };
    }
    if (c == 4) {
        return { KEY_CTRL_D, 0 };
    }
    if (c == '\x1b') {
        char c2, c3;
        if (read(STDIN_FILENO, &c2, 1) != 1 || c2 != '[') return { KEY_NONE, 0 };
        if (read(STDIN_FILENO, &c3, 1) != 1) return { KEY_NONE, 0 };
        if (c3 == 'A') return { KEY_UP, 0 };
        if (c3 == 'B') return { KEY_DOWN, 0 };
        if (c3 == 'C') return { KEY_RIGHT, 0 };
        if (c3 == 'D') return { KEY_LEFT, 0 };
        
        char last = c3;
        while (last < 0x40 || last > 0x7e) {
            if (read(STDIN_FILENO, &last, 1) != 1) break;
        }
        return { KEY_NONE, 0 };
    }
    if (static_cast<unsigned char>(c) >= 32) {
        return { KEY_CHAR, c };
    }
    return { KEY_NONE, 0 };
#endif
}

// Reads one line with arrow-key navigation and history. Falls back to plain
// getline when stdin is not a terminal (piped input, tests).
static std::string readLineRaw(const std::string& prompt, std::vector<std::string>& history,
                               size_t& histIndex, bool& eof) {
    if (!isTerminal()) {
        std::string line;
        if (!std::getline(std::cin, line)) { eof = true; return ""; }
        return line;
    }

#ifndef _WIN32
    if (!g_termiosSaved) {
        tcgetattr(STDIN_FILENO, &g_originalTermios);
        g_termiosSaved = true;
    }
#endif
    setRawMode(true);

    std::string buffer;
    size_t cursor = 0;
    std::cout << prompt << std::flush;

    auto refresh = [&]() {
        std::cout << "\r\x1b[K" << prompt << buffer;
        size_t back = buffer.size() - cursor;
        if (back > 0) std::cout << "\x1b[" << back << "D";
        std::cout << std::flush;
    };

    bool done = false;
    while (!done) {
        InputKey key = getNextKey();

        switch (key.type) {
            case KEY_ENTER:
                std::cout << "\r\x1b[K" << std::endl;
                done = true;
                break;
            case KEY_BACKSPACE:
                if (cursor > 0) {
                    buffer.erase(cursor - 1, 1);
                    cursor--;
                    refresh();
                }
                break;
            case KEY_CTRL_D:
                if (buffer.empty()) {
                    eof = true;
                    std::cout << std::endl;
                    done = true;
                }
                break;
            case KEY_UP:
                if (histIndex > 0) {
                    histIndex--;
                    buffer = history[histIndex];
                    cursor = buffer.size();
                    refresh();
                }
                break;
            case KEY_DOWN:
                if (histIndex < history.size()) {
                    histIndex++;
                    if (histIndex == history.size()) {
                        buffer.clear();
                        cursor = 0;
                    } else {
                        buffer = history[histIndex];
                        cursor = buffer.size();
                    }
                    refresh();
                }
                break;
            case KEY_RIGHT:
                if (cursor < buffer.size()) {
                    cursor++;
                    std::cout << "\x1b[C" << std::flush;
                }
                break;
            case KEY_LEFT:
                if (cursor > 0) {
                    cursor--;
                    std::cout << "\x1b[D" << std::flush;
                }
                break;
            case KEY_CHAR:
                buffer.insert(cursor, 1, key.ch);
                cursor++;
                std::cout << key.ch;
                {
                    size_t back = buffer.size() - cursor;
                    if (back > 0) std::cout << "\x1b[" << back << "D";
                }
                std::cout << std::flush;
                break;
            case KEY_EOF:
                eof = true;
                done = true;
                break;
            case KEY_NONE:
            default:
                break;
        }
    }

    setRawMode(false);
    return buffer;
}

static void runRepl(Interpreter& interpreter) {
    std::cout << "Catherine REPL — write code, it runs as you complete each statement. Ctrl-D to exit.\n";
    std::vector<std::string> history;
    std::string buffer;
    while (true) {
        size_t histIndex = history.size();
        bool eof = false;
        std::string line = readLineRaw(buffer.empty() ? "rine> " : "  ..> ", history, histIndex, eof);
        if (eof) {
            if (!buffer.empty()) {
                try {
                    runSource(interpreter, buffer);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
            }
            std::cout << std::endl;
            break;
        }
        if (!line.empty()) history.push_back(line);
        buffer += line + "\n";
        if (!isCompleteInput(buffer)) continue;
        try {
            runSource(interpreter, buffer);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        buffer.clear();
    }
}

int main(int argc, char* argv[]) {
    // Make sure the terminal is not left in raw mode if the user hits Ctrl-C.
    std::signal(SIGINT, [](int) {
        restoreTermios();
        std::_Exit(130);
    });

    Interpreter interpreter;

    if (argc > 1) {
        std::string filePath = argv[1];

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open source file: " << filePath << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string sourceCode = buffer.str();
        file.close();

        try {
            std::cout << "=== Catherine Interpreter Output (" << filePath << ") ===\n" << std::endl;
            runSource(interpreter, sourceCode);
            std::cout << "\n========================================================" << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            return 1;
        }

        return 0;
    }

    runRepl(interpreter);
    return 0;
}

