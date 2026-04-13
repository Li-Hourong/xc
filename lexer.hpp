#include <string>

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
    std::string name = NULL;
    int line = 0;

    Token(Type type, const std::string& name, int line)
        : type(type), name(name), line(line) {}
    
    void print() const {
        std::cout << "Token [ Type: " << type << ", Name: " << name << "line: " << line <<" ]" << std::endl;
    }

}

enum State
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

Token GetToken(char*& ptr, int line);