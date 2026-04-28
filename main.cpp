#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "[Error] Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }
    std::string srcfile = argv[1];
    std::ifstream file(srcfile);

    if(!file.is_open()) {
        std::cerr << "[Error] Could not open file " << srcfile << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    file.close();

    const char* ptr = content.data();
    int line = 1;
    std::vector<Token> tokens;

    while(*ptr) {
        if(*ptr == ' ' || *ptr == '\n' || *ptr == '\t') {
            if(*ptr == '\n') line++;
            ptr++;
            continue;
        }
        if(*ptr == '/' && *(ptr + 1) == '/') {
            while(*ptr && *ptr != '\n') ptr++;
            continue;
        }
        if(*ptr == '/' && *(ptr + 1) == '*') {
            ptr += 2;
            while(*ptr && !(*(ptr - 1) == '*' && *ptr == '/')) {
                if(*ptr == '\n') line++;
                ptr++;
            }
            if(*ptr) ptr += 2;
            continue;
        }
        Token token = GetToken(ptr, line);
        //token.print();
        tokens.push_back(token);
    }
    
    std::unique_ptr<CompUnit> root = parse(tokens);
    root->print();
    
    return 0;
}