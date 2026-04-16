#include "lexer.hpp"

#include <cstring>
#include <unordered_map>

namespace {
Token::Type ResolveIdentifierType(const std::string& text)
{
    static const std::unordered_map<std::string, Token::Type> keywords = {
        {"int", Token::INT},
        {"void", Token::VOID},
        {"char", Token::CHAR},
        {"if", Token::IF},
        {"else", Token::ELSE},
        {"while", Token::WHILE},
        {"return", Token::RETURN},
        {"read", Token::READ},
        {"write", Token::WRITE}
    };

    const auto it = keywords.find(text);
    return it == keywords.end() ? Token::ID : it->second;
}
}

Token GetToken(const char*& ptr, int line)
{
    char ch = *ptr++;
    switch(ch) {
        case '(': return Token(Token::LPAREN, "(", line);
        case ')': return Token(Token::RPAREN, ")", line);
        case '{': return Token(Token::LBRACE, "{", line);
        case '}': return Token(Token::RBRACE, "}", line);
        case '[': return Token(Token::LBRACK, "[", line);
        case ']': return Token(Token::RBRACK, "]", line);
        case ';': return Token(Token::SEMICOLON, ";", line);
        case ',': return Token(Token::COMMA, ",", line);
    }
    ptr--;

    State state = State::START;
    std::string tokenStr;
    bool invaliddigit = false;

    while(state != State::ACCEPT) {
        ch = *ptr;
        switch(state) {
            case State::START:
            {
                if( *ptr >= 'a' && *ptr <= 'z' || *ptr >= 'A' && *ptr <= 'Z' || *ptr == '_') {
                    state = State::ID; ptr++; tokenStr += ch;
                } else if(*ptr == '0' && *(ptr + 1) == 'b') {
                    state = State::BIN; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) == 'B') {
                    state = State::BIN; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) == 'x') {
                    state = State::HEX; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) == 'X') {
                    state = State::HEX; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) >= '0' && *(ptr + 1) <= '7') {
                    state = State::OCT; ptr++;
                } else if(*ptr >= '0' && *ptr <= '9') {
                    state = State::DEC; ptr++; tokenStr += ch;
                } else if(*ptr == '\'') {
                    state = State::CHARC; ptr++;
                } else if(*ptr == '"') {
                    state = State::STRC; ptr++;
                } else if(strchr("+-*/%=&|!<>.", *ptr)) {
                    state = State::OP;
                } else {
                    state = State::ERROR;
                }
                break;
            }
            case State::ID:
            {
                if( *ptr >= 'a' && *ptr <= 'z' || *ptr >= 'A' && *ptr <= 'Z' || *ptr == '_' || (*ptr >= '0' && *ptr <= '9')) {
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    state = State::ACCEPT;
                    return Token(ResolveIdentifierType(tokenStr), tokenStr, line);
                }
                break;
            }
            case State::BIN:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'z') || 
                   (*ptr >= 'A' && *ptr <= 'Z')) {
                    
                    // 检查是否为非法二进制数字或字母
                    if(!(*ptr == '0' || *ptr == '1')) {
                        invaliddigit = true;  // 标记有非法字符
                    }
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = State::ACCEPT;
                    
                    // 返回结果：如果有非法字符，返回错误类型
                    if(invaliddigit) {
                        return Token(Token::ERROR, tokenStr, line);
                    }
                    return Token(Token::BIN, tokenStr, line);
                }
                break;
            }
            case State::OCT:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'z') || 
                   (*ptr >= 'A' && *ptr <= 'Z')) {
                    
                    // 检查是否为非法二进制数字或字母
                    if(!(*ptr >= '0' && *ptr <= '7')) {
                        invaliddigit = true;  // 标记有非法字符
                    }
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = State::ACCEPT;
                    
                    // 返回结果：如果有非法字符，返回错误类型
                    if(invaliddigit) {
                        return Token(Token::ERROR, tokenStr, line);
                    }
                    return Token(Token::OCT, tokenStr, line);
                }
                break;
            }
            case State::DEC:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'z') || 
                   (*ptr >= 'A' && *ptr <= 'Z')) {
                    
                    // 检查是否为非法二进制数字或字母
                    if(!(*ptr >= '0' && *ptr <= '9')) {
                        invaliddigit = true;  // 标记有非法字符
                    }
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = State::ACCEPT;
                    
                    // 返回结果：如果有非法字符，返回错误类型
                    if(invaliddigit) {
                        return Token(Token::ERROR, tokenStr, line);
                    }
                    return Token(Token::DEC, tokenStr, line);
                }
                break;
            }
            case State::HEX:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'z') || 
                   (*ptr >= 'A' && *ptr <= 'Z')) {
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = State::ACCEPT;
                    return Token(Token::HEX, tokenStr, line);
                }
                break;
            }

            case State::CHARC:
            {
                tokenStr += *ptr++;
                if(*ptr == '\'') {
                    ptr++;
                    state = State::ACCEPT;
                    return Token(Token::CHARC, tokenStr, line);
                } else {
                    state = State::ERROR;
                }
                break;
            }
            case State::STRC:
            {
                if (*ptr == '"') {            // 字符串结束
                    ptr++;
                    return Token(Token::STRC, tokenStr, line);
                } else if (*ptr == '\\') {    // 发现反斜杠
                    ptr++;
                    state = State::ESCAPE; // 跳转到转义处理状态
                } else if (*ptr == '\n' || *ptr == '\0') {
                    state = State::ERROR;           // 字符串未闭合
                } else {
                    tokenStr += *ptr++;       // 普通字符
                }
                break;
            }
                
            case State::OP:
            {
                const char op = *ptr;
                const char next = *(ptr + 1);

                switch(op) {
                    case '+':
                        if(next == '=') { ptr += 2; return Token(Token::PLUS_AS, "+=", line); }
                        ptr++;
                        return Token(Token::PLUS, "+", line);
                    case '-':
                        if(next == '=') { ptr += 2; return Token(Token::MIN_AS, "-=", line); }
                        ptr++;
                        return Token(Token::MIN, "-", line);
                    case '*':
                        if(next == '=') { ptr += 2; return Token(Token::AST_AS, "*=", line); }
                        ptr++;
                        return Token(Token::AST, "*", line);
                    case '/':
                        if(next == '=') { ptr += 2; return Token(Token::DIV_AS, "/=", line); }
                        ptr++;
                        return Token(Token::DIV, "/", line);
                    case '%':
                        if(next == '=') { ptr += 2; return Token(Token::MOD_AS, "%=", line); }
                        ptr++;
                        return Token(Token::MOD, "%", line);
                    case '=':
                        if(next == '=') { ptr += 2; return Token(Token::EQ, "==", line); }
                        ptr++;
                        return Token(Token::ASSIGN, "=", line);
                    case '!':
                        if(next == '=') { ptr += 2; return Token(Token::NEQ, "!=", line); }
                        ptr++;
                        return Token(Token::NOT, "!", line);
                    case '<':
                        if(next == '=') { ptr += 2; return Token(Token::LTE, "<=", line); }
                        ptr++;
                        return Token(Token::LT, "<", line);
                    case '>':
                        if(next == '=') { ptr += 2; return Token(Token::GTE, ">=", line); }
                        ptr++;
                        return Token(Token::GT, ">", line);
                    case '&':
                        if(next == '&') { ptr += 2; return Token(Token::AND, "&&", line); }
                        ptr++;
                        return Token(Token::REF, "&", line);
                    case '|':
                        if(next == '|') { ptr += 2; return Token(Token::OR, "||", line); }
                        ptr++;
                        return Token(Token::ERROR, "|", line);
                    default:
                        ptr++;
                        return Token(Token::ERROR, std::string(1, op), line);
                }
            }
            
            case State::ESCAPE:
            {
                if (*ptr == 'n') {
                    tokenStr += '\n';
                } else if (*ptr == 't') {
                    tokenStr += '\t';
                } else if (*ptr == '\\') {
                    tokenStr += '\\';
                } else if (*ptr == '"') {
                    tokenStr += '"';
                } else if (*ptr == '\'') {
                    tokenStr += '\'';
                } else {
                    state = State::ERROR; // 不支持的转义序列
                }
                if(state != State::ERROR) ptr++;
                state = State::STRC; // 返回字符串状态继续处理
                break;
            }

            case State::ERROR:
            {
                while(*ptr && *ptr != '\n' && *ptr != ' ' && *ptr != '\t') {
                    ptr++;
                }
                return Token(Token::ERROR, std::string(1, ch), line);
            }
        }
    }
}