#pragma once
#include "lexer.hpp"
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
    Typekind kind;
    Type* base = nullptr; // for PTR and ARR
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
};

class CompUnit : public ASTNode
{
public:
    ~CompUnit() override;
    CompUnit() = default;

    std::vector<ASTNode*> units;  // FuncDef or VarDecl
    CompUnit(ASTNode* unit) { add_unit(unit); }
    void add_unit(ASTNode* unit) { units.push_back(unit); }
    std::string to_string() override { return "CompUnit"; }
    std::vector<ASTNode*> get_children() override { return units; }
};

class FuncCall : public ASTNode
{
public:
    std::string ident;
    std::vector<ASTNode*> args;
    FuncCall(const std::string& ident) : ident(ident) {};
    FuncCall(const std::string& ident, const std::vector<ASTNode*>& args) : ident(ident), args(args) {}
    
    std::string to_string() override { 
        return "FuncCall [ " + ident + " arg_num: " + std::to_string(args.size()) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return args; }  
};

class VarDef : public ASTNode
{
public:
    std::string ident;
    Type type;
    ASTNode* init_value; // can be nullptr if no initializer
    VarDef(const std::string& ident, const Type& type) : ident(ident), type(type) {}
    VarDef(const std::string& ident, const Type& type, ASTNode* init_value)
        : ident(ident), type(type), init_value(init_value) {}
    
    std::string to_string() override { 
        return "VarDef [ " + ident + " initialized: " + std::string(init_value != nullptr ? "true" : "false") + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { 
        return init_value ? std::vector<ASTNode*>{init_value} : std::vector<ASTNode*>(); 
    }
};

class VarDecl : public ASTNode
{
public:
    Type base_type;
    std::vector<VarDef*> defs;
    VarDecl(const Type& base_type) : base_type(base_type){}
    ~VarDecl() override {};
    std::string to_string() override { return "VarDecl [ " + base_type.to_string() + " ]"; }

};

class Params : public ASTNode
{
public:
    std::vector<ASTNode*> params;
    Params(VarDecl* p) {add_param(p);}
    ~Params() override {};
    void add_param(VarDecl* p) {params.push_back(p);}
    std::string to_string() override { return "Params [ " + std::to_string(params.size()) + " ]"; }
    std::vector<ASTNode*> get_children() override {
        return params;
    }
};

class FuncDef : public ASTNode
{
public:
    Type return_type;
    std::string ident;
    std::vector<ASTNode*> params;
    FuncDef(Params* p) {params.push_back(p);}
    ~FuncDef() override {};
    std::string to_string() override { return "FuncDef [ " + ident + " return type: " + return_type.to_string() + " ]"; }
    std::vector<ASTNode*> get_children() override {
        return params;
    }
};

class ArrayDef : public ASTNode
{
public:
    std::string ident;
    Type type;
    std::vector<int> shape;
    ASTNode* init_list; // can be nullptr if no initializer

    ArrayDef(const std::string& ident, const Type& type, const std::vector<int>& shape) 
        : ident(ident), type(type), shape(shape) {}
    ArrayDef(const std::string& ident, const Type& type, const std::vector<int>& shape, ASTNode* init_list) 
        : ident(ident), type(type), shape(shape), init_list(init_list) {}
    
    std::string to_string() override { 
        return "ArrayDef [ " + ident + " dim: " +
                std::to_string(shape.size()) + "initialized: " 
                + std::string(init_list != nullptr ? "true" : "false") +" ]"; 
    }
    
    std::vector<ASTNode*> get_children() override { 
        return init_list ? std::vector<ASTNode*>{init_list} : std::vector<ASTNode*>(); 
    }
};

class Expr : public ASTNode
{
public:
    Op op;
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
    std::vector<Expr*> indices;
    Array(const std::string& ident, const std::vector<Expr*>& indices) : ident(ident), indices(indices) {}
    std::string to_string() override { 
        return "Array [ " + ident + " dim: " + std::to_string(indices.size()) + " ]"; 
    }
};

class BinaryExpr : public Expr
{
public:
    ASTNode* left;
    ASTNode* right;
    BinaryExpr(Op op, ASTNode* left, ASTNode* right) : left(left), right(right) { this->op = op; }
    std::string to_string() override { 
        return "BinaryExpr [ op: " + std::string(Token::TypeToString(static_cast<Token::Type>(op))) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return {left, right}; }
};

class UnaryExpr : public Expr
{
public:
    ASTNode* operand;
    UnaryExpr(Op op, ASTNode* operand) : operand(operand) { this->op = op; }
    std::string to_string() override { 
        return "UnaryExpr [ op: " + std::string(Token::TypeToString(static_cast<Token::Type>(op))) + " ]"; 
    }
    std::vector<ASTNode*> get_children() override { return {operand}; }

};

class InitList : public ASTNode
{
public:
    std::vector<ASTNode*> init_values; // can be IntRval, CharRval, StrRval, or nested InitList
    explicit InitList(ASTNode* value) { add_value(value); }
    void add_value(ASTNode* value) { init_values.push_back(value); }
    std::string to_string() override { return "InitList [ size: " + std::to_string(init_values.size()) + " ]"; }
    std::vector<ASTNode*> get_children() override { return init_values; }
};

class Block : public ASTNode
{
public:
    std::vector<ASTNode*> stmts; // Stmt or VarDecl
    Block() {}
    Block(ASTNode* stmt) { add_stmt(stmt); }
    ~Block() override;
    void add_stmt(ASTNode* stmt) { stmts.push_back(stmt); }
    std::string to_string() override { return "Block"; }
    std::vector<ASTNode*> get_children() override { return stmts; }
};

class IfStmt : public ASTNode
{
public:
    Expr* cond;
    ASTNode* then_branch;
    ASTNode* else_branch;
    IfStmt(Expr* cond, ASTNode* then_stmt)
        : cond(cond), then_branch(then_stmt), else_branch(nullptr) {}
    IfStmt(Expr* cond, ASTNode* then_stmt, ASTNode* else_stmt)
      :  cond(cond), then_branch(then_stmt), else_branch(else_stmt) {}

    std::string to_string() override {
        return "IfStmt [ has_else_branch: " + std::string(else_branch ? "true" : "false") +
           " ]";
    }
};

class WhileStmt : public ASTNode
{
public:
    ASTNode* cond;
    ASTNode* body;
    WhileStmt(ASTNode* cond, ASTNode* body) : cond(cond), body(body) {}
    std::string to_string() override { return "WhileStmt"; }
    std::vector<ASTNode*> get_children() override { return {cond, body}; }
};

class ReturnStmt : public ASTNode
{
public:
    ASTNode* ret_expr; // can be nullptr for void functions
    ReturnStmt() : ret_expr(nullptr) {}
    ReturnStmt(ASTNode* ret_expr) : ret_expr(ret_expr) {}
    std::string to_string() override { return "ReturnStmt"; }
    std::vector<ASTNode*> get_children() override { 
        return ret_expr ? std::vector<ASTNode*>{ret_expr} : std::vector<ASTNode*>(); 
    }
};

