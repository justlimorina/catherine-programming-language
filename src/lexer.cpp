#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string& src)
    : source(src), pos(0), currentLine(1) {
    // Register language keywords
    keywords["define"]  = TokenType::KEYWORD_DEFINE;
    keywords["as"]      = TokenType::KEYWORD_AS;
    keywords["number"]  = TokenType::KEYWORD_NUMBER;
    keywords["string"]  = TokenType::KEYWORD_STRING;
    keywords["boolean"] = TokenType::KEYWORD_BOOLEAN;
    keywords["echo"]    = TokenType::KEYWORD_ECHO;
    keywords["listen"]  = TokenType::KEYWORD_LISTEN;
    keywords["if"]      = TokenType::KEYWORD_IF;
    keywords["ef"]      = TokenType::KEYWORD_EF;
    keywords["else"]    = TokenType::KEYWORD_ELSE;
    keywords["while"]   = TokenType::KEYWORD_WHILE;
    keywords["until"]   = TokenType::KEYWORD_UNTIL;
    keywords["repeat"]  = TokenType::KEYWORD_REPEAT;
    keywords["for"]     = TokenType::KEYWORD_FOR;
    keywords["up"]      = TokenType::KEYWORD_UP;
    keywords["down"]    = TokenType::KEYWORD_DOWN;
    keywords["stop"]    = TokenType::KEYWORD_STOP;
    keywords["skip"]    = TokenType::KEYWORD_SKIP;
    keywords["clear"]   = TokenType::KEYWORD_CLEAR;
    keywords["func"]    = TokenType::KEYWORD_FUNC;
    keywords["struct"]  = TokenType::KEYWORD_STRUCT;
    keywords["calc"]    = TokenType::KEYWORD_CALC;
    keywords["and"]     = TokenType::KEYWORD_AND;
    keywords["or"]      = TokenType::KEYWORD_OR;
    keywords["true"]    = TokenType::KEYWORD_TRUE;
    keywords["false"]   = TokenType::KEYWORD_FALSE;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    return source[pos++];
}

bool Lexer::isAtEnd() const {
    return pos >= source.length();
}

bool Lexer::isSmartQuote(size_t i) const {
    return i + 2 < source.length() &&
           (unsigned char)source[i] == 0xE2 &&
           (unsigned char)source[i+1] == 0x80 &&
           ((unsigned char)source[i+2] == 0x9C || (unsigned char)source[i+2] == 0x9D);
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else if (c == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
            // Skip single line // comments until end of line
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::scanNumber() {
    size_t start = pos;
    while (!isAtEnd() && (std::isdigit(peek()) || peek() == '.')) {
        advance();
    }
    std::string val = source.substr(start, pos - start);
    return Token(TokenType::NUMBER_LITERAL, val, currentLine);
}

Token Lexer::scanIdentifierOrKeyword() {
    size_t start = pos;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    std::string text = source.substr(start, pos - start);

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return Token(it->second, text, currentLine);
    }
    return Token(TokenType::IDENTIFIER, text, currentLine);
}

Token Lexer::scanString() {
    if (source[pos] == '"') {
        advance();
    } else if (isSmartQuote(pos)) {
        pos += 3;
    }

    size_t start = pos;
    while (!isAtEnd()) {
        char c = peek();
        if (c == '"') {
            break;
        }
        if (isSmartQuote(pos)) {
            break;
        }
        if (c == '\n') currentLine++;
        advance();
    }

    std::string val = source.substr(start, pos - start);

    // Skip closing quote " or UTF-8 smart quote ”
    if (!isAtEnd()) {
        if (peek() == '"') {
            advance();
        } else if (isSmartQuote(pos)) {
            pos += 3;
        }
    }

    return Token(TokenType::STRING_LITERAL, val, currentLine);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        char c = peek();

        if (c == '\n') {
            tokens.push_back(Token(TokenType::NEWLINE, "\\n", currentLine));
            currentLine++;
            advance();
            continue;
        }

        if (std::isdigit(c)) {
            tokens.push_back(scanNumber());
            continue;
        }

        if (std::isalpha(c) || c == '_') {
            tokens.push_back(scanIdentifierOrKeyword());
            continue;
        }

        // Handle string quotes " or UTF-8 smart quotes “
        if (c == '"' || isSmartQuote(pos)) {
            tokens.push_back(scanString());
            continue;
        }

        // Operators & Symbols
        advance();
        switch (c) {
            case '=':
                if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::EQUAL, "==", currentLine)); }
                else { tokens.push_back(Token(TokenType::ASSIGN, "=", currentLine)); }
                break;
            case '!':
                if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::NOT_EQUAL, "!=", currentLine)); }
                else { tokens.push_back(Token(TokenType::UNKNOWN, "!", currentLine)); }
                break;
            case '<':
                if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", currentLine)); }
                else { tokens.push_back(Token(TokenType::LESS, "<", currentLine)); }
                break;
            case '>':
                if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", currentLine)); }
                else { tokens.push_back(Token(TokenType::GREATER, ">", currentLine)); }
                break;
            case '+': tokens.push_back(Token(TokenType::PLUS, "+", currentLine)); break;
            case '-': tokens.push_back(Token(TokenType::MINUS, "-", currentLine)); break;
            case '*': tokens.push_back(Token(TokenType::STAR, "*", currentLine)); break;
            case '/': tokens.push_back(Token(TokenType::SLASH, "/", currentLine)); break;
            case ',': tokens.push_back(Token(TokenType::COMMA, ",", currentLine)); break;
            case '.': tokens.push_back(Token(TokenType::DOT, ".", currentLine)); break;
            case '(': tokens.push_back(Token(TokenType::LPAREN, "(", currentLine)); break;
            case ')': tokens.push_back(Token(TokenType::RPAREN, ")", currentLine)); break;
            case '{': tokens.push_back(Token(TokenType::LBRACE, "{", currentLine)); break;
            case '}': tokens.push_back(Token(TokenType::RBRACE, "}", currentLine)); break;
            case '[': tokens.push_back(Token(TokenType::LBRACKET, "[", currentLine)); break;
            case ']': tokens.push_back(Token(TokenType::RBRACKET, "]", currentLine)); break;
            default:
                tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), currentLine));
                break;
        }
    }

    tokens.push_back(Token(TokenType::TOKEN_EOF, "EOF", currentLine));
    return tokens;
}
