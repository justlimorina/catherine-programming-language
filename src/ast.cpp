#include "ast.hpp"

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}

void NumberExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "NumberExpr: " << value << "\n";
}

void StringExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "StringExpr: \"" << value << "\"\n";
}

void BooleanExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "BooleanExpr: " << (value ? "true" : "false") << "\n";
}

void VariableExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "VariableExpr: " << name << "\n";
}

void ArrayAccessExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "ArrayAccessExpr:\n";
    printIndent(indent + 1);
    std::cout << "Target:\n";
    arrayExpr->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Index:\n";
    indexExpr->print(indent + 2);
}

void MemberAccessExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "MemberAccessExpr (" << memberName << "):\n";
    objectExpr->print(indent + 1);
}

void BinaryExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "BinaryExpr (" << op << "):\n";
    if (left) left->print(indent + 1);
    if (right) right->print(indent + 1);
}

void CallExprAST::print(int indent) const {
    printIndent(indent);
    std::cout << "CallExpr: " << callee << "()\n";
    for (const auto& arg : args) {
        arg->print(indent + 1);
    }
}

void VarDefineStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "VarDefineStmt (Type: " << typeName << (isArray ? "[]" : "") << "): ";
    for (size_t i = 0; i < names.size(); ++i) {
        std::cout << names[i] << (i + 1 < names.size() ? ", " : "");
    }
    std::cout << "\n";
    if (initialValue) {
        printIndent(indent + 1);
        std::cout << "InitialValue:\n";
        initialValue->print(indent + 2);
    }
}

void StructDefStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "StructDefStmt: " << structName << "\n";
    for (const auto& f : fields) {
        f->print(indent + 1);
    }
}

void AssignStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "AssignStmt:\n";
    printIndent(indent + 1);
    std::cout << "Target:\n";
    if (target) target->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Value:\n";
    if (value) value->print(indent + 2);
}

void EchoStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "EchoStmt:\n";
    if (expr) expr->print(indent + 1);
}

void ListenStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "ListenStmt:\n";
    for (const auto& t : targets) {
        t->print(indent + 1);
    }
}

void BlockStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "BlockStmt:\n";
    for (const auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}

void IfStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "IfStmt:\n";
    for (size_t i = 0; i < branches.size(); ++i) {
        printIndent(indent + 1);
        std::cout << (i == 0 ? "If Branch Condition:\n" : "Ef Branch Condition:\n");
        branches[i].condition->print(indent + 2);
        printIndent(indent + 1);
        std::cout << "Branch Body:\n";
        branches[i].body->print(indent + 2);
    }
    if (elseBranch) {
        printIndent(indent + 1);
        std::cout << "Else Branch Body:\n";
        elseBranch->print(indent + 2);
    }
}

void WhileStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "WhileStmt Condition:\n";
    if (condition) condition->print(indent + 1);
    printIndent(indent);
    std::cout << "While Body:\n";
    if (body) body->print(indent + 1);
}

void UntilStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "UntilStmt Condition:\n";
    if (condition) condition->print(indent + 1);
    printIndent(indent);
    std::cout << "Until Body:\n";
    if (body) body->print(indent + 1);
}

void ForStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "ForStmt:\n";
    printIndent(indent + 1);
    std::cout << "Init:\n";
    if (initStmt) initStmt->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Condition:\n";
    if (condition) condition->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Step:\n";
    if (stepStmt) stepStmt->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Body:\n";
    if (body) body->print(indent + 2);
}

void RepeatStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "RepeatStmt Count:\n";
    if (countExpr) countExpr->print(indent + 1);
    printIndent(indent);
    std::cout << "Repeat Body:\n";
    if (body) body->print(indent + 1);
}

void StopStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "StopStmt\n";
}

void SkipStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "SkipStmt\n";
}

void ClearStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "ClearStmt\n";
}

void FuncDefStmtAST::print(int indent) const {
    printIndent(indent);
    std::cout << "FuncDefStmt: " << name << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        std::cout << params[i] << (i + 1 < params.size() ? ", " : "");
    }
    std::cout << ") as " << returnType << "\n";
    if (body) body->print(indent + 1);
}
