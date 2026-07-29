#include "token.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD_DEFINE: return "KEYWORD_DEFINE";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_NUMBER: return "KEYWORD_NUMBER";
        case TokenType::KEYWORD_STRING: return "KEYWORD_STRING";
        case TokenType::KEYWORD_BOOLEAN: return "KEYWORD_BOOLEAN";
        case TokenType::KEYWORD_ECHO: return "KEYWORD_ECHO";
        case TokenType::KEYWORD_LISTEN: return "KEYWORD_LISTEN";
        case TokenType::KEYWORD_IF: return "KEYWORD_IF";
        case TokenType::KEYWORD_EF: return "KEYWORD_EF";
        case TokenType::KEYWORD_ELSE: return "KEYWORD_ELSE";
        case TokenType::KEYWORD_WHILE: return "KEYWORD_WHILE";
        case TokenType::KEYWORD_UNTIL: return "KEYWORD_UNTIL";
        case TokenType::KEYWORD_REPEAT: return "KEYWORD_REPEAT";
        case TokenType::KEYWORD_FOR: return "KEYWORD_FOR";
        case TokenType::KEYWORD_UP: return "KEYWORD_UP";
        case TokenType::KEYWORD_DOWN: return "KEYWORD_DOWN";
        case TokenType::KEYWORD_STOP: return "KEYWORD_STOP";
        case TokenType::KEYWORD_SKIP: return "KEYWORD_SKIP";
        case TokenType::KEYWORD_CLEAR: return "KEYWORD_CLEAR";
        case TokenType::KEYWORD_FUNC: return "KEYWORD_FUNC";
        case TokenType::KEYWORD_STRUCT: return "KEYWORD_STRUCT";
        case TokenType::KEYWORD_CALC: return "KEYWORD_CALC";
        case TokenType::KEYWORD_AND: return "KEYWORD_AND";
        case TokenType::KEYWORD_OR: return "KEYWORD_OR";
        case TokenType::KEYWORD_TRUE: return "KEYWORD_TRUE";
        case TokenType::KEYWORD_FALSE: return "KEYWORD_FALSE";

        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";

        case TokenType::ASSIGN: return "ASSIGN (=)";
        case TokenType::EQUAL: return "EQUAL (==)";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL (!=)";
        case TokenType::LESS: return "LESS (<)";
        case TokenType::GREATER: return "GREATER (>)";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL (<=)";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL (>=)";
        case TokenType::PLUS: return "PLUS (+)";
        case TokenType::MINUS: return "MINUS (-)";
        case TokenType::STAR: return "STAR (*)";
        case TokenType::SLASH: return "SLASH (/)";
        case TokenType::COMMA: return "COMMA (,)";
        case TokenType::DOT: return "DOT (.)";
        case TokenType::LPAREN: return "LPAREN (()";
        case TokenType::RPAREN: return "RPAREN ())";
        case TokenType::LBRACE: return "LBRACE ({)";
        case TokenType::RBRACE: return "RBRACE (})";
        case TokenType::LBRACKET: return "LBRACKET ([)";
        case TokenType::RBRACKET: return "RBRACKET (])";

        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}
