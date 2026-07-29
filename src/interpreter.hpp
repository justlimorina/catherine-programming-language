#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "ast.hpp"
#include "environment.hpp"
#include "value.hpp"
#include <memory>
#include <unordered_map>

// Signal exceptions for loop control flow
struct StopSignal {};
struct SkipSignal {};

class Interpreter {
public:
    Interpreter();
    void interpret(const std::vector<std::shared_ptr<StmtAST>>& statements);

    Value evaluate(const std::shared_ptr<ExprAST>& expr, std::shared_ptr<Environment> env);
    void execute(const std::shared_ptr<StmtAST>& stmt, std::shared_ptr<Environment> env);

private:
    std::shared_ptr<Environment> globalEnv;
    std::unordered_map<std::string, std::shared_ptr<FuncDefStmtAST>> functions;
    std::unordered_map<std::string, std::shared_ptr<StructDefStmtAST>> structDefs;

    std::string interpolateString(const std::string& input, std::shared_ptr<Environment> env);
    Value callBuiltinOrUserFunction(const std::string& name, const std::vector<Value>& args, std::shared_ptr<Environment> env);
    
    Value createDefaultValueForType(const std::string& typeName);
    Value createDefaultStructInstance(const std::string& structName);

    void assignTarget(const std::shared_ptr<ExprAST>& target, const Value& val, std::shared_ptr<Environment> env);
};

#endif // INTERPRETER_HPP
