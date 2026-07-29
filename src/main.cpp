#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    std::string filePath = "example.txt";
    if (argc > 1) {
        filePath = argv[1];
    }

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
        // Phase 1: Lexing
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        // Phase 2: Parsing
        Parser parser(tokens);
        std::vector<std::shared_ptr<StmtAST>> astStatements = parser.parse();

        // Phase 3: Interpreting
        std::cout << "=== Catherine Interpreter Output (" << filePath << ") ===\n" << std::endl;
        Interpreter interpreter;
        interpreter.interpret(astStatements);
        std::cout << "\n========================================================" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
