#include "parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& t) : tokens(t), current(0) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::TOKEN_EOF;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Parser Error on line " + std::to_string(peek().line) + ": " + message + " (found '" + peek().lexeme + "')");
}

void Parser::skipNewlines() {
    while (check(TokenType::NEWLINE)) {
        advance();
    }
}

std::vector<std::shared_ptr<StmtAST>> Parser::parse() {
    std::vector<std::shared_ptr<StmtAST>> statements;
    while (!isAtEnd()) {
        skipNewlines();
        if (isAtEnd()) break;
        std::shared_ptr<StmtAST> stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        }
    }
    return statements;
}

std::shared_ptr<StmtAST> Parser::parseStatement() {
    skipNewlines();
    if (isAtEnd()) return nullptr;

    int line = peek().line;
    std::shared_ptr<StmtAST> stmt;
    if (match({TokenType::KEYWORD_DEFINE})) stmt = parseDefine();
    else if (match({TokenType::KEYWORD_ECHO})) stmt = parseEcho();
    else if (match({TokenType::KEYWORD_LISTEN})) stmt = parseListen();
    else if (match({TokenType::KEYWORD_IF})) stmt = parseIf();
    else if (match({TokenType::KEYWORD_WHILE})) stmt = parseWhile();
    else if (match({TokenType::KEYWORD_UNTIL})) stmt = parseUntil();
    else if (match({TokenType::KEYWORD_FOR})) stmt = parseFor();
    else if (match({TokenType::KEYWORD_REPEAT})) stmt = parseRepeat();
    else if (match({TokenType::KEYWORD_STOP})) stmt = std::make_shared<StopStmtAST>();
    else if (match({TokenType::KEYWORD_SKIP})) stmt = std::make_shared<SkipStmtAST>();
    else if (match({TokenType::KEYWORD_CLEAR})) stmt = std::make_shared<ClearStmtAST>();
    else if (check(TokenType::IDENTIFIER)) stmt = parseAssignOrCall();
    else {
        advance();
        return nullptr;
    }

    stmt->line = line;
    return stmt;
}

std::shared_ptr<StmtAST> Parser::parseDefine() {
    if (check(TokenType::IDENTIFIER) && current + 2 < tokens.size() &&
        tokens[current + 1].type == TokenType::KEYWORD_AS &&
        tokens[current + 2].type == TokenType::KEYWORD_STRUCT) {
        return parseStructDef();
    }
    if (match({TokenType::KEYWORD_FUNC})) {
        return parseFuncDef();
    }
    return parseVarDef();
}

std::shared_ptr<StmtAST> Parser::parseStructDef() {
    std::string name = tokens[current].lexeme;
    advance(); // name
    advance(); // as
    advance(); // struct
    skipNewlines();
    consume(TokenType::LBRACE, "Expected '{' in struct definition");
    skipNewlines();
    std::vector<std::shared_ptr<VarDefineStmtAST>> fields;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        skipNewlines();
        if (match({TokenType::KEYWORD_DEFINE})) {
            auto fieldStmt = std::dynamic_pointer_cast<VarDefineStmtAST>(parseVarDef());
            if (fieldStmt) fields.push_back(fieldStmt);
        }
        skipNewlines();
    }
    consume(TokenType::RBRACE, "Expected '}' after struct definition");
    return std::make_shared<StructDefStmtAST>(name, fields);
}

std::shared_ptr<StmtAST> Parser::parseFuncDef() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected function name after 'define func'");
    consume(TokenType::LPAREN, "Expected '(' after function name");

    std::vector<std::string> params;
    skipNewlines();
    if (!check(TokenType::RPAREN)) {
        do {
            skipNewlines();
            Token param = consume(TokenType::IDENTIFIER, "Expected parameter name");
            params.push_back(param.lexeme);
            skipNewlines();
        } while (match({TokenType::COMMA}));
    }
    skipNewlines();
    consume(TokenType::RPAREN, "Expected ')' after parameters");

    consume(TokenType::KEYWORD_AS, "Expected 'as' after function signature");
    Token typeToken = advance();

    skipNewlines();
    std::shared_ptr<BlockStmtAST> body = parseBlock();

    return std::make_shared<FuncDefStmtAST>(nameToken.lexeme, params, typeToken.lexeme, body);
}

std::shared_ptr<StmtAST> Parser::parseVarDef() {
    std::vector<std::string> varNames;
    std::shared_ptr<ExprAST> initVal = nullptr;

    do {
        skipNewlines();
        Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name in 'define'");
        varNames.push_back(varToken.lexeme);

        if (match({TokenType::EQUAL, TokenType::ASSIGN})) {
            skipNewlines();
            initVal = parseExpression();
        }
        skipNewlines();
    } while (match({TokenType::COMMA}));

    consume(TokenType::KEYWORD_AS, "Expected 'as' in variable definition");
    skipNewlines();

    Token typeToken = advance();
    if (typeToken.type != TokenType::IDENTIFIER &&
        typeToken.type != TokenType::KEYWORD_STRING &&
        typeToken.type != TokenType::KEYWORD_NUMBER &&
        typeToken.type != TokenType::KEYWORD_BOOLEAN) {
        throw std::runtime_error("Parser Error on line " + std::to_string(typeToken.line) + ": Expected type name in 'define'");
    }

    bool isArray = false;
    if (match({TokenType::LBRACKET})) {
        consume(TokenType::RBRACKET, "Expected ']' after array type");
        isArray = true;
    }

    return std::make_shared<VarDefineStmtAST>(varNames, typeToken.lexeme, isArray, initVal);
}

std::shared_ptr<StmtAST> Parser::parseAssignOrCall() {
    std::shared_ptr<ExprAST> target = parsePrimary();

    if (match({TokenType::ASSIGN, TokenType::EQUAL})) {
        skipNewlines();
        std::shared_ptr<ExprAST> val = parseExpression();
        return std::make_shared<AssignStmtAST>(target, val);
    }

    return std::make_shared<EchoStmtAST>(target);
}

std::shared_ptr<StmtAST> Parser::parseEcho() {
    skipNewlines();
    std::shared_ptr<ExprAST> expr = parseExpression();
    return std::make_shared<EchoStmtAST>(expr);
}

std::shared_ptr<StmtAST> Parser::parseListen() {
    std::vector<std::shared_ptr<ExprAST>> targets;
    do {
        skipNewlines();
        targets.push_back(parsePrimary());
        skipNewlines();
    } while (match({TokenType::COMMA}));

    return std::make_shared<ListenStmtAST>(targets);
}

std::shared_ptr<BlockStmtAST> Parser::parseBlock() {
    consume(TokenType::LBRACE, "Expected '{' to start block");
    skipNewlines();

    std::vector<std::shared_ptr<StmtAST>> statements;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        std::shared_ptr<StmtAST> stmt = parseStatement();
        if (stmt) statements.push_back(stmt);
        skipNewlines();
    }

    consume(TokenType::RBRACE, "Expected '}' to end block");
    return std::make_shared<BlockStmtAST>(statements);
}

std::shared_ptr<StmtAST> Parser::parseIf() {
    std::vector<IfBranch> branches;

    skipNewlines();
    std::shared_ptr<ExprAST> cond = parseExpression();
    skipNewlines();
    std::shared_ptr<BlockStmtAST> body = parseBlock();
    branches.push_back({cond, body});

    skipNewlines();
    while (match({TokenType::KEYWORD_EF})) {
        skipNewlines();
        std::shared_ptr<ExprAST> efCond = parseExpression();
        skipNewlines();
        std::shared_ptr<BlockStmtAST> efBody = parseBlock();
        branches.push_back({efCond, efBody});
        skipNewlines();
    }

    std::shared_ptr<BlockStmtAST> elseBody = nullptr;
    if (match({TokenType::KEYWORD_ELSE})) {
        skipNewlines();
        elseBody = parseBlock();
    }

    return std::make_shared<IfStmtAST>(branches, elseBody);
}

std::shared_ptr<StmtAST> Parser::parseWhile() {
    skipNewlines();
    std::shared_ptr<ExprAST> cond = parseExpression();
    skipNewlines();
    std::shared_ptr<BlockStmtAST> body = parseBlock();
    return std::make_shared<WhileStmtAST>(cond, body);
}

std::shared_ptr<StmtAST> Parser::parseUntil() {
    skipNewlines();
    std::shared_ptr<ExprAST> cond = parseExpression();
    skipNewlines();
    std::shared_ptr<BlockStmtAST> body = parseBlock();
    return std::make_shared<UntilStmtAST>(cond, body);
}

std::shared_ptr<StmtAST> Parser::parseFor() {
    skipNewlines();
    std::shared_ptr<StmtAST> initStmt = parseStatement();
    consume(TokenType::COMMA, "Expected ',' after for loop initialization");
    skipNewlines();

    std::shared_ptr<ExprAST> condExpr = parseExpression();
    consume(TokenType::COMMA, "Expected ',' after for loop condition");
    skipNewlines();

    std::shared_ptr<StmtAST> stepStmt = nullptr;
    if (match({TokenType::KEYWORD_UP})) {
        if (auto assign = std::dynamic_pointer_cast<AssignStmtAST>(initStmt)) {
            stepStmt = std::make_shared<AssignStmtAST>(
                assign->target,
                std::make_shared<BinaryExprAST>("+", assign->target, std::make_shared<NumberExprAST>(1))
            );
        }
    } else if (match({TokenType::KEYWORD_DOWN})) {
        if (auto assign = std::dynamic_pointer_cast<AssignStmtAST>(initStmt)) {
            stepStmt = std::make_shared<AssignStmtAST>(
                assign->target,
                std::make_shared<BinaryExprAST>("-", assign->target, std::make_shared<NumberExprAST>(1))
            );
        }
    } else {
        stepStmt = parseStatement();
    }

    skipNewlines();
    std::shared_ptr<BlockStmtAST> body = parseBlock();
    return std::make_shared<ForStmtAST>(initStmt, condExpr, stepStmt, body);
}

std::shared_ptr<StmtAST> Parser::parseRepeat() {
    skipNewlines();
    std::shared_ptr<ExprAST> countExpr = parseExpression();
    skipNewlines();
    std::shared_ptr<BlockStmtAST> body = parseBlock();
    return std::make_shared<RepeatStmtAST>(countExpr, body);
}

// ---------------- EXPRESSION PARSER (Operator Precedence) ----------------

std::shared_ptr<ExprAST> Parser::parseExpression() {
    return parseLogicalOr();
}

std::shared_ptr<ExprAST> Parser::parseLogicalOr() {
    std::shared_ptr<ExprAST> expr = parseLogicalAnd();
    while (match({TokenType::KEYWORD_OR})) {
        std::string op = previous().lexeme;
        skipNewlines();
        std::shared_ptr<ExprAST> right = parseLogicalAnd();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseLogicalAnd() {
    std::shared_ptr<ExprAST> expr = parseEquality();
    while (match({TokenType::KEYWORD_AND})) {
        std::string op = previous().lexeme;
        skipNewlines();
        std::shared_ptr<ExprAST> right = parseEquality();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseEquality() {
    std::shared_ptr<ExprAST> expr = parseComparison();
    while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
        std::string op = previous().lexeme;
        skipNewlines();
        std::shared_ptr<ExprAST> right = parseComparison();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseComparison() {
    std::shared_ptr<ExprAST> expr = parseTerm();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        std::string op = previous().lexeme;
        skipNewlines();
        std::shared_ptr<ExprAST> right = parseTerm();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseTerm() {
    std::shared_ptr<ExprAST> expr = parseFactor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        skipNewlines();
        std::shared_ptr<ExprAST> right = parseFactor();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseFactor() {
    std::shared_ptr<ExprAST> expr = parseUnary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        std::string op = previous().lexeme;
        skipNewlines();
        std::shared_ptr<ExprAST> right = parseUnary();
        expr = std::make_shared<BinaryExprAST>(op, expr, right);
    }
    return expr;
}

std::shared_ptr<ExprAST> Parser::parseUnary() {
    skipNewlines();
    if (match({TokenType::MINUS})) {
        std::shared_ptr<ExprAST> operand = parseUnary();
        return std::make_shared<BinaryExprAST>("-", std::make_shared<NumberExprAST>(0), operand);
    }
    return parsePrimary();
}

std::shared_ptr<ExprAST> Parser::parsePrimary() {
    skipNewlines();

    if (match({TokenType::NUMBER_LITERAL})) {
        return std::make_shared<NumberExprAST>(std::stod(previous().lexeme));
    }

    if (match({TokenType::STRING_LITERAL})) {
        return std::make_shared<StringExprAST>(previous().lexeme);
    }

    if (match({TokenType::KEYWORD_TRUE})) {
        return std::make_shared<BooleanExprAST>(true);
    }

    if (match({TokenType::KEYWORD_FALSE})) {
        return std::make_shared<BooleanExprAST>(false);
    }

    if (match({TokenType::LBRACKET})) {
        std::vector<std::shared_ptr<ExprAST>> elements;
        skipNewlines();
        if (!check(TokenType::RBRACKET)) {
            do {
                skipNewlines();
                elements.push_back(parseExpression());
                skipNewlines();
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RBRACKET, "Expected ']' after array literal");
        return std::make_shared<ArrayLiteralExprAST>(elements);
    }

    if (match({TokenType::KEYWORD_CALC})) {
        consume(TokenType::LPAREN, "Expected '(' after 'calc'");
        skipNewlines();
        std::shared_ptr<ExprAST> expr = parseExpression();
        skipNewlines();
        consume(TokenType::RPAREN, "Expected ')' after calc expression");
        return std::make_shared<CallExprAST>("calc", std::vector<std::shared_ptr<ExprAST>>{expr});
    }

    if (match({TokenType::IDENTIFIER, TokenType::KEYWORD_NUMBER, TokenType::KEYWORD_STRING, TokenType::KEYWORD_BOOLEAN})) {
        std::string name = previous().lexeme;
        std::shared_ptr<ExprAST> expr = std::make_shared<VariableExprAST>(name);

        while (true) {
            if (match({TokenType::LPAREN})) {
                std::vector<std::shared_ptr<ExprAST>> args;
                skipNewlines();
                if (!check(TokenType::RPAREN)) {
                    do {
                        skipNewlines();
                        args.push_back(parseExpression());
                        skipNewlines();
                    } while (match({TokenType::COMMA}));
                }
                skipNewlines();
                consume(TokenType::RPAREN, "Expected ')' after function arguments");
                if (auto varNode = std::dynamic_pointer_cast<VariableExprAST>(expr)) {
                    expr = std::make_shared<CallExprAST>(varNode->name, args);
                }
            } else if (match({TokenType::LBRACKET})) {
                skipNewlines();
                std::shared_ptr<ExprAST> indexExpr = parseExpression();
                skipNewlines();
                consume(TokenType::RBRACKET, "Expected ']' after array index");
                expr = std::make_shared<ArrayAccessExprAST>(expr, indexExpr);
            } else if (match({TokenType::DOT})) {
                Token memberToken = consume(TokenType::IDENTIFIER, "Expected member name after '.'");
                expr = std::make_shared<MemberAccessExprAST>(expr, memberToken.lexeme);
            } else {
                break;
            }
        }

        return expr;
    }

    if (match({TokenType::LPAREN})) {
        skipNewlines();
        std::shared_ptr<ExprAST> expr = parseExpression();
        skipNewlines();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return std::make_shared<CallExprAST>("group", std::vector<std::shared_ptr<ExprAST>>{expr});
    }

    throw std::runtime_error("Parser Error on line " + std::to_string(peek().line) + ": Unexpected expression token '" + peek().lexeme + "'");
}
