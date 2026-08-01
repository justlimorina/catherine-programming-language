#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// Base class for all Abstract Syntax Tree (AST) nodes
class ASTNode {
public:
    int line = 0;
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Base class for all Expressions (evaluates to a value)
class ExprAST : public ASTNode {};

// Base class for all Statements (performs an action)
class StmtAST : public ASTNode {};

// ------------------- EXPRESSIONS -------------------

// Number Literal Expression (e.g., 42, 3.14)
class NumberExprAST : public ExprAST {
public:
    double value;
    explicit NumberExprAST(double val) : value(val) {}
    void print(int indent = 0) const override;
};

// String Literal Expression (e.g., "Hello World")
class StringExprAST : public ExprAST {
public:
    std::string value;
    explicit StringExprAST(std::string val) : value(std::move(val)) {}
    void print(int indent = 0) const override;
};

// Boolean Literal Expression (e.g., true, false)
class BooleanExprAST : public ExprAST {
public:
    bool value;
    explicit BooleanExprAST(bool val) : value(val) {}
    void print(int indent = 0) const override;
};

// Variable Expression (e.g., a, count)
class VariableExprAST : public ExprAST {
public:
    std::string name;
    explicit VariableExprAST(std::string n) : name(std::move(n)) {}
    void print(int indent = 0) const override;
};

// Array Access Expression (e.g., arr[i], sv[i])
class ArrayAccessExprAST : public ExprAST {
public:
    std::shared_ptr<ExprAST> arrayExpr;
    std::shared_ptr<ExprAST> indexExpr;

    ArrayAccessExprAST(std::shared_ptr<ExprAST> arr, std::shared_ptr<ExprAST> idx)
        : arrayExpr(std::move(arr)), indexExpr(std::move(idx)) {}
    void print(int indent = 0) const override;
};

// Member Access Expression (e.g., student.name, sv[i].gpa)
class MemberAccessExprAST : public ExprAST {
public:
    std::shared_ptr<ExprAST> objectExpr;
    std::string memberName;

    MemberAccessExprAST(std::shared_ptr<ExprAST> obj, std::string member)
        : objectExpr(std::move(obj)), memberName(std::move(member)) {}
    void print(int indent = 0) const override;
};

// Binary Expression (e.g., a + b, x == 3, c and d)
class BinaryExprAST : public ExprAST {
public:
    std::string op;
    std::shared_ptr<ExprAST> left;
    std::shared_ptr<ExprAST> right;

    BinaryExprAST(std::string o, std::shared_ptr<ExprAST> l, std::shared_ptr<ExprAST> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
    void print(int indent = 0) const override;
};

// Function Call Expression (e.g., sum(a, b), calc(a + b))
class CallExprAST : public ExprAST {
public:
    std::string callee;
    std::vector<std::shared_ptr<ExprAST>> args;

    CallExprAST(std::string name, std::vector<std::shared_ptr<ExprAST>> arguments)
        : callee(std::move(name)), args(std::move(arguments)) {}
    void print(int indent = 0) const override;
};

// ------------------- STATEMENTS -------------------

// Variable Definition Statement (e.g., define a, b as number, define sv as Student[])
class VarDefineStmtAST : public StmtAST {
public:
    std::vector<std::string> names;
    std::string typeName;
    bool isArray;
    std::shared_ptr<ExprAST> initialValue;

    VarDefineStmtAST(std::vector<std::string> n, std::string t, bool arr = false, std::shared_ptr<ExprAST> init = nullptr)
        : names(std::move(n)), typeName(std::move(t)), isArray(arr), initialValue(std::move(init)) {}
    void print(int indent = 0) const override;
};

// Struct Definition Statement (e.g., define Student as struct { define name as string ... })
class StructDefStmtAST : public StmtAST {
public:
    std::string structName;
    std::vector<std::shared_ptr<VarDefineStmtAST>> fields;

    StructDefStmtAST(std::string name, std::vector<std::shared_ptr<VarDefineStmtAST>> f)
        : structName(std::move(name)), fields(std::move(f)) {}
    void print(int indent = 0) const override;
};

// Variable Assignment Statement (e.g., a = 1, sv[i].name = "Alice")
class AssignStmtAST : public StmtAST {
public:
    std::shared_ptr<ExprAST> target;
    std::shared_ptr<ExprAST> value;

    AssignStmtAST(std::shared_ptr<ExprAST> t, std::shared_ptr<ExprAST> v)
        : target(std::move(t)), value(std::move(v)) {}
    void print(int indent = 0) const override;
};

// Echo Output Statement (e.g., echo "Hello", echo a)
class EchoStmtAST : public StmtAST {
public:
    std::shared_ptr<ExprAST> expr;

    explicit EchoStmtAST(std::shared_ptr<ExprAST> e) : expr(std::move(e)) {}
    void print(int indent = 0) const override;
};

// Listen Input Statement (e.g., listen d, listen sv[i].name)
class ListenStmtAST : public StmtAST {
public:
    std::vector<std::shared_ptr<ExprAST>> targets;

    explicit ListenStmtAST(std::vector<std::shared_ptr<ExprAST>> t) : targets(std::move(t)) {}
    void print(int indent = 0) const override;
};

// Block Statement (e.g., { stmt1; stmt2; })
class BlockStmtAST : public StmtAST {
public:
    std::vector<std::shared_ptr<StmtAST>> statements;

    explicit BlockStmtAST(std::vector<std::shared_ptr<StmtAST>> stmts)
        : statements(std::move(stmts)) {}
    void print(int indent = 0) const override;
};

// If Condition Branch struct
struct IfBranch {
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<BlockStmtAST> body;
};

// If / Ef / Else Statement
class IfStmtAST : public StmtAST {
public:
    std::vector<IfBranch> branches;
    std::shared_ptr<BlockStmtAST> elseBranch;

    IfStmtAST(std::vector<IfBranch> b, std::shared_ptr<BlockStmtAST> e)
        : branches(std::move(b)), elseBranch(std::move(e)) {}
    void print(int indent = 0) const override;
};

// While Loop Statement (e.g., while count < 5 { ... })
class WhileStmtAST : public StmtAST {
public:
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<BlockStmtAST> body;

    WhileStmtAST(std::shared_ptr<ExprAST> cond, std::shared_ptr<BlockStmtAST> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

// Until Loop Statement (e.g., until count == 10 { ... })
class UntilStmtAST : public StmtAST {
public:
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<BlockStmtAST> body;

    UntilStmtAST(std::shared_ptr<ExprAST> cond, std::shared_ptr<BlockStmtAST> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

// For Loop Statement (e.g., for i = 0, i < soLuong, up { ... })
class ForStmtAST : public StmtAST {
public:
    std::shared_ptr<StmtAST> initStmt;
    std::shared_ptr<ExprAST> condition;
    std::shared_ptr<StmtAST> stepStmt;
    std::shared_ptr<BlockStmtAST> body;

    ForStmtAST(std::shared_ptr<StmtAST> init, std::shared_ptr<ExprAST> cond, std::shared_ptr<StmtAST> step, std::shared_ptr<BlockStmtAST> b)
        : initStmt(std::move(init)), condition(std::move(cond)), stepStmt(std::move(step)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

// Repeat Loop Statement (e.g., repeat 10 { ... })
class RepeatStmtAST : public StmtAST {
public:
    std::shared_ptr<ExprAST> countExpr;
    std::shared_ptr<BlockStmtAST> body;

    RepeatStmtAST(std::shared_ptr<ExprAST> count, std::shared_ptr<BlockStmtAST> b)
        : countExpr(std::move(count)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

// Stop Statement (break)
class StopStmtAST : public StmtAST {
public:
    void print(int indent = 0) const override;
};

// Skip Statement (continue)
class SkipStmtAST : public StmtAST {
public:
    void print(int indent = 0) const override;
};

// Clear Screen Statement (e.g., clear)
class ClearStmtAST : public StmtAST {
public:
    void print(int indent = 0) const override;
};

// Function Definition Statement (e.g., define func hello(x, y) as number { ... })
class FuncDefStmtAST : public StmtAST {
public:
    std::string name;
    std::vector<std::string> params;
    std::string returnType;
    std::shared_ptr<BlockStmtAST> body;

    FuncDefStmtAST(std::string n, std::vector<std::string> p, std::string ret, std::shared_ptr<BlockStmtAST> b)
        : name(std::move(n)), params(std::move(p)), returnType(std::move(ret)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

#endif // AST_HPP
