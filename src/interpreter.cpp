#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <cmath>
#include <iostream>
#include <sstream>

Interpreter::Interpreter() {
    globalEnv = std::make_shared<Environment>();
}

Value Interpreter::createDefaultValueForType(const std::string& typeName) {
    if (structDefs.find(typeName) != structDefs.end()) {
        return createDefaultStructInstance(typeName);
    }
    if (typeName == "string") return Value(std::string(""));
    if (typeName == "boolean") return Value(false);
    return Value(0.0);
}

Value Interpreter::createDefaultStructInstance(const std::string& structName) {
    std::unordered_map<std::string, Value> members;
    auto it = structDefs.find(structName);
    if (it != structDefs.end()) {
        for (const auto& fieldDef : it->second->fields) {
            for (const auto& fname : fieldDef->names) {
                members[fname] = createDefaultValueForType(fieldDef->typeName);
            }
        }
    }
    return Value(structName, members);
}

std::string Interpreter::interpolateString(const std::string& input, std::shared_ptr<Environment> env) {
    std::string result;
    size_t len = input.length();
    for (size_t i = 0; i < len; ++i) {
        if (input[i] == '!' && i + 1 < len && input[i + 1] == '{') {
            size_t start = i + 2;
            size_t end = input.find('}', start);
            if (end != std::string::npos) {
                std::string exprStr = input.substr(start, end - start);
                Value val;
                
                // Fast path for simple variable names
                if (env->contains(exprStr)) {
                    val = env->get(exprStr);
                } else {
                    // Evaluate inline expressions or function calls inside !{...}
                    try {
                        Lexer lex(exprStr);
                        auto tokens = lex.tokenize();
                        Parser par(tokens);
                        auto expr = par.parseExpression();
                        if (expr) {
                            val = evaluate(expr, env);
                        } else {
                            val = env->get(exprStr);
                        }
                    } catch (...) {
                        val = env->get(exprStr);
                    }
                }
                result += val.toString();
                i = end;
                continue;
            }
        }
        result += input[i];
    }
    return result;
}

Value Interpreter::callBuiltinOrUserFunction(const std::string& name, const std::vector<Value>& args, std::shared_ptr<Environment> env) {
    if (name == "group" && !args.empty()) {
        return args[0];
    }
    if (name == "calc" && !args.empty()) {
        return args[0];
    }
    if (name == "sqrt" && !args.empty()) {
        if (args[0].numberValue < 0) throw std::runtime_error("Runtime Error: Cannot compute sqrt of negative number");
        return Value(std::sqrt(args[0].numberValue));
    }

    // Built-in arithmetic functions
    if (name == "sum" && args.size() >= 2) return Value(args[0].numberValue + args[1].numberValue);
    if (name == "sub" && args.size() >= 2) return Value(args[0].numberValue - args[1].numberValue);
    if (name == "multi" && args.size() >= 2) return Value(args[0].numberValue * args[1].numberValue);
    if (name == "div" && args.size() >= 2) {
        if (args[1].numberValue == 0) throw std::runtime_error("Runtime Error: Division by zero in div()");
        return Value(args[0].numberValue / args[1].numberValue);
    }
    if (name == "idiv" && args.size() >= 2) {
        if (args[1].numberValue == 0) throw std::runtime_error("Runtime Error: Division by zero in idiv()");
        return Value(std::floor(args[0].numberValue / args[1].numberValue));
    }
    if (name == "mod" && args.size() >= 2) {
        if (args[1].numberValue == 0) throw std::runtime_error("Runtime Error: Division by zero in mod()");
        return Value(std::fmod(args[0].numberValue, args[1].numberValue));
    }

    // User-defined function
    auto it = functions.find(name);
    if (it != functions.end()) {
        auto funcDef = it->second;
        auto localEnv = std::make_shared<Environment>(globalEnv);

        // Bind parameters
        for (size_t i = 0; i < funcDef->params.size() && i < args.size(); ++i) {
            localEnv->define(funcDef->params[i], args[i]);
        }

        // Execute function body
        execute(funcDef->body, localEnv);

        // Return value assigned to function name inside function body
        return localEnv->get(name);
    }

    throw std::runtime_error("Runtime Error: Undefined function '" + name + "'");
}

Value Interpreter::evaluate(const std::shared_ptr<ExprAST>& expr, std::shared_ptr<Environment> env) {
    if (!expr) return Value();

    if (auto num = std::dynamic_pointer_cast<NumberExprAST>(expr)) {
        return Value(num->value);
    }

    if (auto str = std::dynamic_pointer_cast<StringExprAST>(expr)) {
        return Value(str->value);
    }

    if (auto b = std::dynamic_pointer_cast<BooleanExprAST>(expr)) {
        return Value(b->value);
    }

    if (auto var = std::dynamic_pointer_cast<VariableExprAST>(expr)) {
        return env->get(var->name);
    }

    if (auto arrAcc = std::dynamic_pointer_cast<ArrayAccessExprAST>(expr)) {
        Value arrVal = evaluate(arrAcc->arrayExpr, env);
        Value idxVal = evaluate(arrAcc->indexExpr, env);
        int idx = static_cast<int>(idxVal.numberValue);
        if (arrVal.type != ValueType::ARRAY) {
            throw std::runtime_error("Runtime Error: Cannot index a non-array value");
        }
        if (idx < 0 || idx >= static_cast<int>(arrVal.arrayValue.size())) {
            throw std::runtime_error("Runtime Error: Array index " + std::to_string(idx) +
                                     " out of bounds (array size " + std::to_string(arrVal.arrayValue.size()) + ")");
        }
        return arrVal.arrayValue[idx];
    }

    if (auto memAcc = std::dynamic_pointer_cast<MemberAccessExprAST>(expr)) {
        Value objVal = evaluate(memAcc->objectExpr, env);
        if (objVal.type == ValueType::STRUCT) {
            auto it = objVal.structMembers.find(memAcc->memberName);
            if (it != objVal.structMembers.end()) {
                return it->second;
            }
        }
        return Value();
    }

    if (auto call = std::dynamic_pointer_cast<CallExprAST>(expr)) {
        std::vector<Value> argValues;
        for (const auto& arg : call->args) {
            argValues.push_back(evaluate(arg, env));
        }
        return callBuiltinOrUserFunction(call->callee, argValues, env);
    }

    if (auto bin = std::dynamic_pointer_cast<BinaryExprAST>(expr)) {
        Value leftVal = evaluate(bin->left, env);
        Value rightVal = evaluate(bin->right, env);

        if (bin->op == "+") {
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() + rightVal.toString());
            }
            return Value(leftVal.numberValue + rightVal.numberValue);
        }
        if (bin->op == "-") return Value(leftVal.numberValue - rightVal.numberValue);
        if (bin->op == "*") return Value(leftVal.numberValue * rightVal.numberValue);
        if (bin->op == "/") {
            if (rightVal.numberValue == 0) throw std::runtime_error("Runtime Error: Division by zero");
            return Value(leftVal.numberValue / rightVal.numberValue);
        }

        if (bin->op == "==") {
            if (leftVal.type == ValueType::BOOLEAN || rightVal.type == ValueType::BOOLEAN) {
                return Value(leftVal.isTruthy() == rightVal.isTruthy());
            }
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() == rightVal.toString());
            }
            return Value(leftVal.numberValue == rightVal.numberValue);
        }
        if (bin->op == "!=") {
            if (leftVal.type == ValueType::BOOLEAN || rightVal.type == ValueType::BOOLEAN) {
                return Value(leftVal.isTruthy() != rightVal.isTruthy());
            }
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() != rightVal.toString());
            }
            return Value(leftVal.numberValue != rightVal.numberValue);
        }
        if (bin->op == "<") {
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() < rightVal.toString());
            }
            return Value(leftVal.numberValue < rightVal.numberValue);
        }
        if (bin->op == ">") {
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() > rightVal.toString());
            }
            return Value(leftVal.numberValue > rightVal.numberValue);
        }
        if (bin->op == "<=") {
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() <= rightVal.toString());
            }
            return Value(leftVal.numberValue <= rightVal.numberValue);
        }
        if (bin->op == ">=") {
            if (leftVal.type == ValueType::STRING || rightVal.type == ValueType::STRING) {
                return Value(leftVal.toString() >= rightVal.toString());
            }
            return Value(leftVal.numberValue >= rightVal.numberValue);
        }

        if (bin->op == "and") return Value(leftVal.isTruthy() && rightVal.isTruthy());
        if (bin->op == "or") return Value(leftVal.isTruthy() || rightVal.isTruthy());
    }

    return Value();
}

void Interpreter::assignTarget(const std::shared_ptr<ExprAST>& target, const Value& val, std::shared_ptr<Environment> env) {
    if (auto var = std::dynamic_pointer_cast<VariableExprAST>(target)) {
        env->assign(var->name, val);
        return;
    }

    if (auto arrAcc = std::dynamic_pointer_cast<ArrayAccessExprAST>(target)) {
        if (auto varArr = std::dynamic_pointer_cast<VariableExprAST>(arrAcc->arrayExpr)) {
            Value arrVal = env->get(varArr->name);
            Value idxVal = evaluate(arrAcc->indexExpr, env);
            int idx = static_cast<int>(idxVal.numberValue);

            if (arrVal.type != ValueType::ARRAY) {
                arrVal = Value(std::vector<Value>{});
            }

            if (idx >= static_cast<int>(arrVal.arrayValue.size())) {
                arrVal.arrayValue.resize(idx + 1);
            }
            arrVal.arrayValue[idx] = val;
            env->assign(varArr->name, arrVal);
            return;
        }
    }

    if (auto memAcc = std::dynamic_pointer_cast<MemberAccessExprAST>(target)) {
        // Case: sv[i].name = val
        if (auto arrAcc = std::dynamic_pointer_cast<ArrayAccessExprAST>(memAcc->objectExpr)) {
            if (auto varArr = std::dynamic_pointer_cast<VariableExprAST>(arrAcc->arrayExpr)) {
                Value arrVal = env->get(varArr->name);
                Value idxVal = evaluate(arrAcc->indexExpr, env);
                int idx = static_cast<int>(idxVal.numberValue);

                if (arrVal.type != ValueType::ARRAY) {
                    arrVal = Value(std::vector<Value>{});
                }

                if (idx >= static_cast<int>(arrVal.arrayValue.size())) {
                    arrVal.arrayValue.resize(idx + 1);
                }

                Value& elementStruct = arrVal.arrayValue[idx];
                if (elementStruct.type != ValueType::STRUCT) {
                    elementStruct = Value("", std::unordered_map<std::string, Value>{});
                }

                elementStruct.structMembers[memAcc->memberName] = val;
                env->assign(varArr->name, arrVal);
                return;
            }
        }
        // Case: student.name = val
        if (auto varObj = std::dynamic_pointer_cast<VariableExprAST>(memAcc->objectExpr)) {
            Value objVal = env->get(varObj->name);
            if (objVal.type == ValueType::STRUCT) {
                objVal.structMembers[memAcc->memberName] = val;
                env->assign(varObj->name, objVal);
                return;
            }
        }
    }
}

namespace {

struct LineAnnotatedError : std::runtime_error {
    LineAnnotatedError(int line, const std::string& msg)
        : std::runtime_error("Line " + std::to_string(line) + ": " + msg) {}
};

} // namespace

void Interpreter::execute(const std::shared_ptr<StmtAST>& stmt, std::shared_ptr<Environment> env) {
    if (!stmt) return;

    try {
    if (auto def = std::dynamic_pointer_cast<VarDefineStmtAST>(stmt)) {
        Value initVal = def->initialValue ? evaluate(def->initialValue, env) : Value();

        for (const auto& name : def->names) {
            if (def->initialValue) {
                env->define(name, initVal);
            } else if (def->isArray) {
                env->define(name, Value(std::vector<Value>{}));
            } else {
                env->define(name, createDefaultValueForType(def->typeName));
            }
        }
        return;
    }

    if (auto structDef = std::dynamic_pointer_cast<StructDefStmtAST>(stmt)) {
        structDefs[structDef->structName] = structDef;
        return;
    }

    if (auto assign = std::dynamic_pointer_cast<AssignStmtAST>(stmt)) {
        Value val = evaluate(assign->value, env);
        assignTarget(assign->target, val, env);
        return;
    }

    if (auto echo = std::dynamic_pointer_cast<EchoStmtAST>(stmt)) {
        Value val = evaluate(echo->expr, env);
        if (val.type == ValueType::STRING) {
            std::cout << interpolateString(val.stringValue, env) << std::endl;
        } else {
            std::cout << val.toString() << std::endl;
        }
        return;
    }

    if (auto listen = std::dynamic_pointer_cast<ListenStmtAST>(stmt)) {
        for (const auto& target : listen->targets) {
            std::string line;
            if (std::getline(std::cin, line)) {
                if (line == "true") {
                    assignTarget(target, Value(true), env);
                    continue;
                }
                if (line == "false") {
                    assignTarget(target, Value(false), env);
                    continue;
                }
                try {
                    size_t idx;
                    double d = std::stod(line, &idx);
                    if (idx == line.length()) {
                        assignTarget(target, Value(d), env);
                        continue;
                    }
                } catch (...) {}
                assignTarget(target, Value(line), env);
            }
        }
        return;
    }

    if (auto block = std::dynamic_pointer_cast<BlockStmtAST>(stmt)) {
        auto blockEnv = std::make_shared<Environment>(env);
        for (const auto& s : block->statements) {
            execute(s, blockEnv);
        }
        return;
    }

    if (auto ifStmt = std::dynamic_pointer_cast<IfStmtAST>(stmt)) {
        for (const auto& branch : ifStmt->branches) {
            Value condVal = evaluate(branch.condition, env);
            if (condVal.isTruthy()) {
                execute(branch.body, env);
                return;
            }
        }
        if (ifStmt->elseBranch) {
            execute(ifStmt->elseBranch, env);
        }
        return;
    }

    if (auto whileStmt = std::dynamic_pointer_cast<WhileStmtAST>(stmt)) {
        while (evaluate(whileStmt->condition, env).isTruthy()) {
            try {
                execute(whileStmt->body, env);
            } catch (const SkipSignal&) {
                continue;
            } catch (const StopSignal&) {
                break;
            }
        }
        return;
    }

    if (auto untilStmt = std::dynamic_pointer_cast<UntilStmtAST>(stmt)) {
        while (!evaluate(untilStmt->condition, env).isTruthy()) {
            try {
                execute(untilStmt->body, env);
            } catch (const SkipSignal&) {
                continue;
            } catch (const StopSignal&) {
                break;
            }
        }
        return;
    }

    if (auto forStmt = std::dynamic_pointer_cast<ForStmtAST>(stmt)) {
        if (forStmt->initStmt) execute(forStmt->initStmt, env);
        while (forStmt->condition ? evaluate(forStmt->condition, env).isTruthy() : true) {
            try {
                execute(forStmt->body, env);
            } catch (const SkipSignal&) {
                // Continue to step
            } catch (const StopSignal&) {
                break;
            }
            if (forStmt->stepStmt) execute(forStmt->stepStmt, env);
        }
        return;
    }

    if (auto repeatStmt = std::dynamic_pointer_cast<RepeatStmtAST>(stmt)) {
        Value countVal = evaluate(repeatStmt->countExpr, env);
        int times = static_cast<int>(countVal.numberValue);
        for (int i = 0; i < times; ++i) {
            try {
                execute(repeatStmt->body, env);
            } catch (const SkipSignal&) {
                continue;
            } catch (const StopSignal&) {
                break;
            }
        }
        return;
    }

    if (auto funcDef = std::dynamic_pointer_cast<FuncDefStmtAST>(stmt)) {
        functions[funcDef->name] = funcDef;
        return;
    }

    if (std::dynamic_pointer_cast<StopStmtAST>(stmt)) {
        throw StopSignal();
    }

    if (std::dynamic_pointer_cast<SkipStmtAST>(stmt)) {
        throw SkipSignal();
    }

    if (std::dynamic_pointer_cast<ClearStmtAST>(stmt)) {
        std::cout << "\033[2J\033[1;1H" << std::flush;
        return;
    }
    } catch (const LineAnnotatedError&) {
        throw;
    } catch (const std::runtime_error& e) {
        throw LineAnnotatedError(stmt->line, e.what());
    }
}

void Interpreter::interpret(const std::vector<std::shared_ptr<StmtAST>>& statements) {
    for (const auto& stmt : statements) {
        execute(stmt, globalEnv);
    }
}
