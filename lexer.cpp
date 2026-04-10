#include "lexer.h"

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
                    state = CHAR; ptr++;
                } else if(*ptr == '"') {
                    state = STR; ptr++;
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
                    return Token(Token::ID, tokenStr, line);
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

            case CHAR:
            {
                tokenStr += *ptr++;
                if(*ptr == '\'') {
                    ptr++;
                    state = ACCEPT;
                    return Token(Token::CHAR, tokenStr, line);
                } else {
                    state = ERROR;
                }
                break;
            }
            case STR:
            {
                if (*ptr == '"') {            // 字符串结束
                    ptr++;
                    return Token(Token::STR, tokenStr, line);
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
                state = STR; // 返回字符串状态继续处理
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