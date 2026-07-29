#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>

// Token types in the Catherine programming language
enum class TokenType {
    // Keywords
    KEYWORD_DEFINE,
    KEYWORD_AS,
    KEYWORD_NUMBER,
    KEYWORD_STRING,
    KEYWORD_BOOLEAN,
    KEYWORD_ECHO,
    KEYWORD_LISTEN,
    KEYWORD_IF,
    KEYWORD_EF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_UNTIL,
    KEYWORD_REPEAT,
    KEYWORD_FOR,
    KEYWORD_UP,
    KEYWORD_DOWN,
    KEYWORD_STOP,
    KEYWORD_SKIP,
    KEYWORD_CLEAR,
    KEYWORD_FUNC,
    KEYWORD_STRUCT,
    KEYWORD_CALC,
    KEYWORD_AND,
    KEYWORD_OR,
    KEYWORD_TRUE,
    KEYWORD_FALSE,

    // Identifiers & Literals
    IDENTIFIER,         // Variable or function names
    NUMBER_LITERAL,     // Numbers (e.g., 1, 3.14)
    STRING_LITERAL,     // Strings (e.g., "Hello World")

    // Operators & Symbols
    ASSIGN,             // =
    EQUAL,              // ==
    NOT_EQUAL,          // !=
    LESS,               // <
    GREATER,            // >
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    COMMA,              // ,
    DOT,                // .
    LPAREN,             // (
    RPAREN,             // )
    LBRACE,             // {
    RBRACE,             // }
    LBRACKET,           // [
    RBRACKET,           // ]

    // Endings & Errors
    NEWLINE,            // Newline character
    TOKEN_EOF,          // End of file
    UNKNOWN             // Unknown or invalid token
};

// Struct representing a single Token
struct Token {
    TokenType type;
    std::string lexue;  // Actual text content
    int line;           // Line number in source file

    Token(TokenType t, std::string lex, int l)
        : type(t), lexue(lex), line(l) {}
};

// Converts a TokenType enum value to a human-readable string
std::string tokenTypeToString(TokenType type);

#endif // TOKEN_HPP
