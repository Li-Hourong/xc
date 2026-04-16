#pragma once
#include <string>
#include <iostream>

class Token
{
public:
    enum Type
    {
        INT, VOID, CHAR,
        IF, ELSE, WHILE, RETURN,
        READ, WRITE,
        ID,
        BIN, DEC, HEX, OCT,
        CHARC, STRC,
        PLUS, MIN, AST, DIV, MOD,
        PLUS_AS, MIN_AS, AST_AS, DIV_AS, MOD_AS,
        AND, OR, NOT, EQ, NEQ, LT, GT, LTE, GTE,
        REF, ASSIGN,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACK,
        RBRACK,
        SEMICOLON,
        COMMA,
        ERROR
    };

    Type type;
    std::string name = "";
    int line = 0;

    Token(Type type, const std::string& name, int line)
        : type(type), name(name), line(line) {}

    static const char* TypeToString(Type type) {
        switch (type) {
            case INT: return "INT";
            case VOID: return "VOID";
            case CHAR: return "CHAR";
            case IF: return "IF";
            case ELSE: return "ELSE";
            case WHILE: return "WHILE";
            case RETURN: return "RETURN";
            case READ: return "READ";
            case WRITE: return "WRITE";
            case ID: return "ID";
            case BIN: return "BIN";
            case DEC: return "DEC";
            case HEX: return "HEX";
            case OCT: return "OCT";
            case CHARC: return "CHARC";
            case STRC: return "STRC";
            case PLUS: return "PLUS";
            case MIN: return "MIN";
            case AST: return "AST";
            case DIV: return "DIV";
            case MOD: return "MOD";
            case PLUS_AS: return "PLUS_AS";
            case MIN_AS: return "MIN_AS";
            case AST_AS: return "AST_AS";
            case DIV_AS: return "DIV_AS";
            case MOD_AS: return "MOD_AS";
            case AND: return "AND";
            case OR: return "OR";
            case NOT: return "NOT";
            case EQ: return "EQ";
            case NEQ: return "NEQ";
            case LT: return "LT";
            case GT: return "GT";
            case LTE: return "LTE";
            case GTE: return "GTE";
            case REF: return "REF";
            case ASSIGN: return "ASSIGN";
            case LPAREN: return "LPAREN";
            case RPAREN: return "RPAREN";
            case LBRACE: return "LBRACE";
            case RBRACE: return "RBRACE";
            case LBRACK: return "LBRACK";
            case RBRACK: return "RBRACK";
            case SEMICOLON: return "SEMICOLON";
            case COMMA: return "COMMA";
            case ERROR: return "ERROR";
        }
        return "UNKNOWN";
    }
    
    void print() const {
        std::cout << "Token [ Type: " << TypeToString(type) << " ; Name: " << name << " ; line: " << line <<" ]" << std::endl;
    }

};

enum class State
{
    START,
    ID,
    BIN, DEC, HEX, OCT,
    CHARC, STRC,
    OP,
    ACCEPT,
    ESCAPE,
    ERROR
};

Token GetToken(const char*& ptr, int line);