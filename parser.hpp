#pragma once
#include "lexer.hpp"
#include <memory>
#include <string>
#include <vector>

enum Precedence {
    PREC_NONE,
    PREC_ASSIGN,     // =
    PREC_LOGICAL_OR, // ||
    PREC_LOGICAL_AND,// &&
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * / %
    PREC_UNARY,      // ! - &
    PREC_CALL,       // () []
};

enum Op {
    PLUS, MIN, AST, DIV, MOD,
    PLUS_AS, MIN_AS, AST_AS, DIV_AS, MOD_AS,
    AND, OR, NOT, EQ, NEQ, LT, GT, LTE, GTE,
    REF, ASSIGN
};

enum class Typekind{
    INT, VOID, CHAR,
    PTR,ARR
};

class Type
{
public:
    Typekind kind = Typekind::INT;
    std::shared_ptr<Type> base = nullptr; // for PTR and ARR
    int arr_size = 0; // for ARR
    std::string to_string() const;
};

class ASTNode
{
public:
    int start_line;
    int end_line;
    virtual std::vector<ASTNode*> get_children() { return std::vector<ASTNode*>(); }
    virtual void print(std::string prefix = "", std::string info_prefix = "");
    virtual std::string to_string() = 0;
    
    ASTNode() : start_line(0), end_line(0) {}
    virtual ~ASTNode() = default;

protected:
    template <typename T>
    static std::vector<ASTNode*> raw_children(const std::vector<std::unique_ptr<T>>& nodes) {
        std::vector<ASTNode*> result;
        result.reserve(nodes.size());
        for (const auto& n : nodes) {
            result.push_back(n.get());
        }
        return result;
    }
};

class CompUnit : public ASTNode
{
public:
    ~CompUnit() override = default;
    CompUnit() = default;

    std::vector<std::unique_ptr<ASTNode>> units;  // FuncDef or VarDecl
    explicit CompUnit(std::unique_ptr<ASTNode> unit) { add_unit(std::move(unit)); }
    void add_unit(std::unique_ptr<ASTNode> unit) { units.push_back(std::move(unit)); }
    std::string to_string() override { return "CompUnit"; }
    std::vector<ASTNode*> get_children() override { return raw_children(units); }
};

class FuncCall : public ASTNode
{
public:
    std::string ident;
    std::vector<std::unique_ptr<ASTNode>> args;
    explicit FuncCall(const std::string& ident) : ident(ident) {}
    FuncCall(const std::string& ident, std::vector<std::unique_ptr<ASTNode>> args)
        : ident(ident), args(std::move(args)) {}
    
    std::string to_string() override { 
        return "FuncCall [ " + ident + " arg_num: " + std::to_string(args.size()) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return raw_children(args); }
};

class VarDef : public ASTNode
{
public:
    std::string ident;
    Type type;
    std::unique_ptr<ASTNode> init_value = nullptr; // can be nullptr if no initializer
    VarDef(const std::string& ident, const Type& type) : ident(ident), type(type) {}
    VarDef(const std::string& ident, const Type& type, std::unique_ptr<ASTNode> init_value)
        : ident(ident), type(type), init_value(std::move(init_value)) {}
    
    std::string to_string() override { 
        return "VarDef [ " + ident + " initialized: " + std::string(init_value != nullptr ? "true" : "false") + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { 
        return init_value ? std::vector<ASTNode*>{init_value.get()} : std::vector<ASTNode*>();
    }
};

class VarDecl : public ASTNode
{
public:
    Type base_type;
    std::vector<std::unique_ptr<VarDef>> defs;
    VarDecl(const Type& base_type) : base_type(base_type){}
    ~VarDecl() override = default;
    std::string to_string() override { return "VarDecl [ " + base_type.to_string() + " ]"; }
    void add_def(std::unique_ptr<VarDef> def) { defs.push_back(std::move(def)); }
    std::vector<ASTNode*> get_children() override {
        return raw_children(defs);
    }
};

class Params : public ASTNode
{
public:
    std::vector<std::unique_ptr<VarDecl>> params;
    Params() = default;
    explicit Params(std::unique_ptr<VarDecl> p) { add_param(std::move(p)); }
    ~Params() override = default;
    void add_param(std::unique_ptr<VarDecl> p) { params.push_back(std::move(p)); }
    std::string to_string() override { return "Params [ " + std::to_string(params.size()) + " ]"; }
    std::vector<ASTNode*> get_children() override {
        return raw_children(params);
    }
};

class ArrayDef : public ASTNode
{
public:
    std::string ident;
    Type type;
    std::vector<int> shape;
    std::unique_ptr<ASTNode> init_list = nullptr; // can be nullptr if no initializer

    ArrayDef(const std::string& ident, const Type& type, const std::vector<int>& shape) 
        : ident(ident), type(type), shape(shape) {}
    ArrayDef(const std::string& ident, const Type& type, const std::vector<int>& shape, std::unique_ptr<ASTNode> init_list)
        : ident(ident), type(type), shape(shape), init_list(std::move(init_list)) {}
    
    std::string to_string() override { 
        return "ArrayDef [ " + ident + " dim: " +
                std::to_string(shape.size()) + "initialized: " 
                + std::string(init_list != nullptr ? "true" : "false") +" ]"; 
    }
    
    std::vector<ASTNode*> get_children() override { 
        return init_list ? std::vector<ASTNode*>{init_list.get()} : std::vector<ASTNode*>();
    }
};

class Expr : public ASTNode
{
public:
    Op op = ASSIGN;
    Expr() = default;
};

class IntRval : public ASTNode
{
public:
    int value;
    IntRval(int value) : value(value) {}
    std::string to_string() override { return "IntRval [ " + std::to_string(value) + " ]"; }
};

class CharRval : public ASTNode
{
public:
    char value;
    CharRval(char value) : value(value) {}
    std::string to_string() override { return "CharRval [ '" + std::string(1, value) + "' ]"; }
};

class StrRval : public ASTNode
{
public:
    std::string value;
    StrRval(const std::string& value) : value(value) {}
    std::string to_string() override { return "StrRval [ \"" + value + "\" ]"; }
};

class Lval : public ASTNode
{
public:
    std::string ident;
    Lval(const std::string& ident) : ident(ident) {}
    std::string to_string() override { 
        return "Lval [ " + ident +  " ]"; 
    }
};

class Array : public ASTNode
{
public:
    std::string ident;
    std::vector<std::unique_ptr<Expr>> indices;
    Array(const std::string& ident, std::vector<std::unique_ptr<Expr>> indices)
        : ident(ident), indices(std::move(indices)) {}
    std::string to_string() override { 
        return "Array [ " + ident + " dim: " + std::to_string(indices.size()) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return raw_children(indices); }
};

class BinaryExpr : public Expr
{
public:
    std::unique_ptr<ASTNode> left = nullptr;
    std::unique_ptr<ASTNode> right = nullptr;
    BinaryExpr(Op op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : left(std::move(left)), right(std::move(right)) { this->op = op; }
    std::string to_string() override { 
        return "BinaryExpr [ op: " + std::string(Token::TypeToString(static_cast<Token::Type>(op))) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return {left.get(), right.get()}; }
};

class UnaryExpr : public Expr
{
public:
    std::unique_ptr<ASTNode> operand = nullptr;
    UnaryExpr(Op op, std::unique_ptr<ASTNode> operand)
        : operand(std::move(operand)) { this->op = op; }
    std::string to_string() override { 
        return "UnaryExpr [ op: " + std::string(Token::TypeToString(static_cast<Token::Type>(op))) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return {operand.get()}; }

};

class InitList : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> init_values; // can be IntRval, CharRval, StrRval, or nested InitList
    InitList() = default;
    explicit InitList(std::unique_ptr<ASTNode> value) { add_value(std::move(value)); }
    void add_value(std::unique_ptr<ASTNode> value) { init_values.push_back(std::move(value)); }
    std::string to_string() override { return "InitList [ size: " + std::to_string(init_values.size()) + " ]"; }
    std::vector<ASTNode*> get_children() override { return raw_children(init_values); }
};

class Block : public ASTNode
{
public:
    std::vector<std::unique_ptr<ASTNode>> stmts; // Stmt or VarDecl
    Block() {}
    explicit Block(std::unique_ptr<ASTNode> stmt) { add_stmt(std::move(stmt)); }
    ~Block() override = default;
    void add_stmt(std::unique_ptr<ASTNode> stmt) { stmts.push_back(std::move(stmt)); }
    std::string to_string() override { return "Block"; }
    std::vector<ASTNode*> get_children() override { return raw_children(stmts); }
};

class IfStmt : public ASTNode
{
public:
    std::unique_ptr<Expr> cond = nullptr;
    std::unique_ptr<ASTNode> then_branch = nullptr;
    std::unique_ptr<ASTNode> else_branch = nullptr;
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<ASTNode> then_stmt)
        : cond(std::move(cond)), then_branch(std::move(then_stmt)), else_branch(nullptr) {}
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<ASTNode> then_stmt, std::unique_ptr<ASTNode> else_stmt)
      : cond(std::move(cond)), then_branch(std::move(then_stmt)), else_branch(std::move(else_stmt)) {}

    std::string to_string() override {
        return "IfStmt [ has_else_branch: " + std::string(else_branch ? "true" : "false") +
           " ]";
    }
    std::vector<ASTNode*> get_children() override {
        if (else_branch) {
            return {cond.get(), then_branch.get(), else_branch.get()};
        }
        return {cond.get(), then_branch.get()};
    }
};

class WhileStmt : public ASTNode
{
public:
    std::unique_ptr<ASTNode> cond = nullptr;
    std::unique_ptr<ASTNode> body = nullptr;
    WhileStmt(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> body)
        : cond(std::move(cond)), body(std::move(body)) {}
    std::string to_string() override { return "WhileStmt"; }
    std::vector<ASTNode*> get_children() override { return {cond.get(), body.get()}; }
};

class ReturnStmt : public ASTNode
{
public:
    std::unique_ptr<ASTNode> ret_expr = nullptr; // can be nullptr for void functions
    ReturnStmt() = default;
    explicit ReturnStmt(std::unique_ptr<ASTNode> ret_expr) : ret_expr(std::move(ret_expr)) {}
    std::string to_string() override { return "ReturnStmt"; }
    std::vector<ASTNode*> get_children() override { 
        return ret_expr ? std::vector<ASTNode*>{ret_expr.get()} : std::vector<ASTNode*>();
    }
};

class FuncDef : public ASTNode
{
public:
    Type return_type;
    std::string ident;
    std::unique_ptr<Params> params = nullptr;
    std::unique_ptr<Block> body = nullptr;
    FuncDef(std::unique_ptr<Params> p, std::unique_ptr<Block> b)
        : params(std::move(p)), body(std::move(b)) {}
    ~FuncDef() override = default;
    std::string to_string() override { return "FuncDef [ " + ident + " return type: " + return_type.to_string() + " ]"; }
    std::vector<ASTNode*> get_children() override {
        return std::vector<ASTNode*>{params.get(), body.get()};
    }
};

std::unique_ptr<CompUnit> parse(std::vector<Token>);