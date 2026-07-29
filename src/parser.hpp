#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<StmtAST>> parse();
    std::shared_ptr<ExprAST> parseExpression();

private:
    std::vector<Token> tokens;
    size_t current;

    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    void skipNewlines();

    // Statement parsers
    std::shared_ptr<StmtAST> parseStatement();
    std::shared_ptr<StmtAST> parseDefine();
    std::shared_ptr<StmtAST> parseEcho();
    std::shared_ptr<StmtAST> parseListen();
    std::shared_ptr<StmtAST> parseIf();
    std::shared_ptr<StmtAST> parseWhile();
    std::shared_ptr<StmtAST> parseUntil();
    std::shared_ptr<StmtAST> parseFor();
    std::shared_ptr<StmtAST> parseRepeat();
    std::shared_ptr<BlockStmtAST> parseBlock();
    std::shared_ptr<StmtAST> parseAssignOrCall();

    // Expression parsers (Precedence Hierarchy)
    std::shared_ptr<ExprAST> parseLogicalOr();
    std::shared_ptr<ExprAST> parseLogicalAnd();
    std::shared_ptr<ExprAST> parseEquality();
    std::shared_ptr<ExprAST> parseComparison();
    std::shared_ptr<ExprAST> parseTerm();
    std::shared_ptr<ExprAST> parseFactor();
    std::shared_ptr<ExprAST> parsePrimary();
};

#endif // PARSER_HPP
