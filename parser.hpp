#pragma once
#include "lexer.hpp"
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
    std::vector<ASTNode*> units;  // FuncDef or VarDecl
    CompUnit(ASTNode* unit) { add_unit(unit); }
    void add_unit(ASTNode* unit) { units.push_back(unit); }
    std::string to_string() override { return "CompUnit"; }
    std::vector<ASTNode*> get_children() override { return units; }
};

class FuncDef : public ASTNode
{
public:

};

class FuncCall : public ASTNode
{
public:
    
};

class VarDecl : public ASTNode
{
public:
    
};

class VarDef : public ASTNode
{
public:
    std::string ident;
    Type type;
    VarDef(const std::string& ident, const Type& type) : ident(ident), type(type) {}
};

class Expr : public ASTNode
{
public:
    Op op;
    Expr() = default;
};

class Stmt : public ASTNode
{
public:

};

class IntRval : public ASTNode
{
public:
};

class CharRval : public ASTNode
{
public:
};

class StrRval : public ASTNode
{
public:
};

class Lval : public ASTNode
{
public:
};

class BinaryExpr : public Expr
{
public:

};

class UnaryExpr : public Expr
{
public:

};

class InitList : public ASTNode
{
public:

};

class Block : public ASTNode
{
public:

};

class IfStmt : public ASTNode
{
public:

};

class WhileStmt : public ASTNode
{
public:

};

class ReturnStmt : public ASTNode
{
public:

};

