#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

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

static void runRepl(Interpreter& interpreter) {
    std::cout << "Catherine REPL — write code, it runs as you complete each statement. Ctrl-D to exit.\n";
    std::string buffer, line;
    while (true) {
        std::cout << (buffer.empty() ? "rine> " : "  ..> ");
        if (!std::getline(std::cin, line)) {
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
