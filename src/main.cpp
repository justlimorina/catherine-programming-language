#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>

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

// ---- Minimal terminal line editor (readline-lite, no external dependency) ----

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

// Reads one line with arrow-key navigation and history. Falls back to plain
// getline when stdin is not a terminal (piped input, tests).
static std::string readLineRaw(const std::string& prompt, std::vector<std::string>& history,
                               size_t& histIndex, bool& eof) {
    if (!isatty(STDIN_FILENO)) {
        std::string line;
        if (!std::getline(std::cin, line)) { eof = true; return ""; }
        return line;
    }

    if (!g_termiosSaved) {
        tcgetattr(STDIN_FILENO, &g_originalTermios);
        g_termiosSaved = true;
    }
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
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) { eof = true; break; }

        if (c == '\n' || c == '\r') {
            std::cout << "\r\x1b[K" << std::endl;
            done = true;
        } else if (c == 127 || c == 8) {          // Backspace
            if (cursor > 0) { buffer.erase(cursor - 1, 1); cursor--; refresh(); }
        } else if (c == 4) {                       // Ctrl-D: exit on empty line
            if (buffer.empty()) { eof = true; std::cout << std::endl; done = true; }
        } else if (c == '\x1b') {                  // Escape sequence (arrow keys, etc.)
            char c2, c3;
            if (read(STDIN_FILENO, &c2, 1) != 1 || c2 != '[') continue;
            if (read(STDIN_FILENO, &c3, 1) != 1) continue;
            if (c3 == 'A') {                       // Up: history
                if (histIndex > 0) { histIndex--; buffer = history[histIndex]; cursor = buffer.size(); refresh(); }
            } else if (c3 == 'B') {                // Down: forward history
                if (histIndex < history.size()) {
                    histIndex++;
                    if (histIndex == history.size()) { buffer.clear(); cursor = 0; }
                    else { buffer = history[histIndex]; cursor = buffer.size(); }
                    refresh();
                }
            } else if (c3 == 'C') {                // Right
                if (cursor < buffer.size()) { cursor++; std::cout << "\x1b[C" << std::flush; }
            } else if (c3 == 'D') {                // Left
                if (cursor > 0) { cursor--; std::cout << "\x1b[D" << std::flush; }
            } else {                               // Home/End/etc.: discard rest of the sequence
                char last = c3;
                while (last < 0x40 || last > 0x7e) {
                    if (read(STDIN_FILENO, &last, 1) != 1) break;
                }
            }
        } else if (c >= 32 && c <= 126) {          // Printable character
            buffer.insert(cursor, 1, c);
            cursor++;
            std::cout << c;
            size_t back = buffer.size() - cursor;
            if (back > 0) std::cout << "\x1b[" << back << "D";
            std::cout << std::flush;
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
