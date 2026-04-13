#include "lexer.h"

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

Token GetToken(char*& ptr, int line)
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

    State state = START;
    std::string tokenStr;
    bool invaliddigit = false;

    while(state != ACCEPT) {
        ch = *ptr;
        switch(state) {
            case START:
            {
                if( *ptr >= 'a' && *ptr <= 'z' || *ptr >= 'A' && *ptr <= 'Z' || *ptr == '_') {
                    state = ID; ptr++; tokenStr += ch;
                } else if(*ptr == '0' && *(ptr + 1) == 'b') {
                    state = BIN; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) == 'B') {
                    state = BIN; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) == 'x') {
                    state = HEX; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) == 'X') {
                    state = HEX; ptr += 2;
                } else if(*ptr == '0' && *(ptr + 1) >= '0' && *(ptr + 1) <= '7') {
                    state = OCT; ptr++;
                } else if(*ptr >= '0' && *ptr <= '9') {
                    state = DEC; ptr++; tokenStr += ch;
                } else if(*ptr == '\'') {
                    state = CHARC; ptr++;
                } else if(*ptr == '"') {
                    state = STRC; ptr++;
                } else if(strchr("+-*/%=&|!<>.", *ptr)) {
                    state = OP; ptr++; tokenStr += ch;
                } else {
                    state = ERROR;
                }
                break;
            }
            case ID:
            {
                if( *ptr >= 'a' && *ptr <= 'z' || *ptr >= 'A' && *ptr <= 'Z' || *ptr == '_' || (*ptr >= '0' && *ptr <= '9')) {
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    state = ACCEPT;
                    return Token(ResolveIdentifierType(tokenStr), tokenStr, line);
                }
                break;
            }
            case BIN:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'f') || 
                   (*ptr >= 'A' && *ptr <= 'F')) {
                    
                    // 检查是否为非法二进制数字或字母
                    if(!(*ptr == '0' || *ptr == '1')) {
                        invaliddigit = true;  // 标记有非法字符
                    }
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = ACCEPT;
                    
                    // 返回结果：如果有非法字符，返回错误类型
                    if(invaliddigit) {
                        return Token(Token::ERROR, tokenStr, line);
                    }
                    return Token(Token::BIN, tokenStr, line);
                }
                break;
            }
            case OCT:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'f') || 
                   (*ptr >= 'A' && *ptr <= 'F')) {
                    
                    // 检查是否为非法二进制数字或字母
                    if(!(*ptr >= '0' && *ptr <= '7')) {
                        invaliddigit = true;  // 标记有非法字符
                    }
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = ACCEPT;
                    
                    // 返回结果：如果有非法字符，返回错误类型
                    if(invaliddigit) {
                        return Token(Token::ERROR, tokenStr, line);
                    }
                    return Token(Token::OCT, tokenStr, line);
                }
                break;
            }
            case DEC:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'f') || 
                   (*ptr >= 'A' && *ptr <= 'F')) {
                    
                    // 检查是否为非法二进制数字或字母
                    if(!(*ptr >= '0' && *ptr <= '9')) {
                        invaliddigit = true;  // 标记有非法字符
                    }
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = ACCEPT;
                    
                    // 返回结果：如果有非法字符，返回错误类型
                    if(invaliddigit) {
                        return Token(Token::ERROR, tokenStr, line);
                    }
                    return Token(Token::DEC, tokenStr, line);
                }
                break;
            }
            case HEX:
            {
                if((*ptr >= '0' && *ptr <= '9') || 
                   (*ptr >= 'a' && *ptr <= 'f') || 
                   (*ptr >= 'A' && *ptr <= 'F')) {
                    
                    tokenStr += *ptr;
                    ptr++;
                } else {
                    // 遇到非数字非字母，结束识别
                    state = ACCEPT;
                    return Token(Token::HEX, tokenStr, line);
                }
                break;
            }

            case CHARC:
            {
                tokenStr += *ptr++;
                if(*ptr == '\'') {
                    ptr++;
                    state = ACCEPT;
                    return Token(Token::CHARC, tokenStr, line);
                } else {
                    state = ERROR;
                }
                break;
            }
            case STRC:
            {
                if (*ptr == '"') {            // 字符串结束
                    ptr++;
                    return Token(Token::STRC, tokenStr, line);
                } else if (*ptr == '\\') {    // 发现反斜杠
                    ptr++;
                    state = ESCAPE; // 跳转到转义处理状态
                } else if (*ptr == '\n' || *ptr == '\0') {
                    state = ERROR;           // 字符串未闭合
                } else {
                    tokenStr += *ptr++;       // 普通字符
                }
                break;
            }
                
            case OP:
            {
                if(strchr("+-*/%=!><", *ptr)) {
                    tokenStr += *ptr; // 添加当前字符
                    if(*(ptr + 1) == '=') {
                        tokenStr += '=';  // 添加等号
                        ptr += 2;         // 跳过两个字符
                        switch(*ptr) {
                            case '+': return Token(Token::PLUS_AS, tokenStr, line);
                            case '-': return Token(Token::MIN_AS, tokenStr, line);
                            case '*': return Token(Token::AST_AS, tokenStr, line);
                            case '/': return Token(Token::DIV_AS, tokenStr, line);
                            case '%': return Token(Token::MOD_AS, tokenStr, line);
                            case '=': return Token(Token::EQ, tokenStr, line);
                            case '!': return Token(Token::NEQ, tokenStr, line);
                            case '<': return Token(Token::LTE, tokenStr, line);
                            case '>': return Token(Token::GTE, tokenStr, line);
                        }
                    } else {
                        ptr++;            // 只跳过当前字符
                        switch(ch) {
                            case '+': return Token(Token::PLUS, tokenStr, line);
                            case '-': return Token(Token::MIN, tokenStr, line);
                            case '*': return Token(Token::AST, tokenStr, line);
                            case '/': return Token(Token::DIV, tokenStr, line);
                            case '%': return Token(Token::MOD, tokenStr, line);
                            case '=': return Token(Token::ASSIGN, tokenStr, line);
                            case '!': return Token(Token::NOT, tokenStr, line);
                            case '<': return Token(Token::LT, tokenStr, line);
                            case '>': return Token(Token::GT, tokenStr, line);
                        }
                    }
                }
                else if( *ptr == '&') {
                    if(*(ptr + 1) == '&') {
                        ptr += 2;
                        return Token(Token::AND, "&&", line);
                    }
                    else return Token(Token::REF, "&", line);
                }
                else if( *ptr == '|' && *(ptr + 1) == '|') {
                    ptr += 2;
                    return Token(Token::OR, "||", line);
                }
                
            }
            
            case ESCAPE:
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
                    state = ERROR; // 不支持的转义序列
                }
                if(state != ERROR) ptr++;
                state = STRC; // 返回字符串状态继续处理
                break;
            }

            case ERROR:
            {
                while(*ptr && *ptr != '\n' && *ptr != ' ' && *ptr != '\t') {
                    ptr++;
                }
                return Token(Token::ERROR, std::string(1, ch), line);
            }
        }
    }
}