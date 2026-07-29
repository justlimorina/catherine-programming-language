#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int currentLine;

    std::unordered_map<std::string, TokenType> keywords;

    char peek() const;
    char advance();
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token scanNumber();
    Token scanIdentifierOrKeyword();
    Token scanString();
};

#endif // LEXER_HPP
