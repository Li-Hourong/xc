#include "parser.hpp"
#include <memory>
#include <utility>

void ASTNode::print(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << " ( line " << start_line 
            << " - " << end_line << " )" << std::endl;
    auto children = get_children();
    if (children.size() == 1) {
        children[0]->print(prefix + "    ", prefix + " └─ ");
    } else {
        for (size_t i = 0; i < children.size(); i++) {
            if (i == children.size() - 1) {
                children[i]->print(prefix + "    ", prefix + " └─ ");
            } else {
                children[i]->print(prefix + " │  ", prefix + " ├─ ");
            }
        }
    }
}

std::string Type::to_string() const {
    switch (kind) {
        case Typekind::INT: return "int";
        case Typekind::VOID: return "void";
        case Typekind::CHAR: return "char";
        case Typekind::PTR:
            return (base ? base->to_string() : std::string("<?>")) + "*";
        case Typekind::ARR:
            return (base ? base->to_string() : std::string("<?>")) + "[" + (arr_size < 0 ? "" : std::to_string(arr_size)) + "]";
    }
    return "<?>";
}

Token Parser::current(){
    if (pos < tokens.size()) 
        return tokens[pos]; 
    return Token(Token::ERROR, std::string(""), -1);
}

bool Parser::is_end(){
    return pos >= tokens.size() || tokens[pos].type == Token::ERROR;
}

bool Parser::expect(Token::Type t, std::string msg){
    if(tokens[pos].type == t){
        pos++;
        return true;
    }
    std::cerr << "Parse error: expected " << Token::TypeToString(t) << " at line "
          << (pos < tokens.size() ? tokens[pos].line : -1) << " : " << msg << std::endl;
    while (!is_end() && tokens[pos].type != Token::SEMICOLON && tokens[pos].type != Token::RBRACE) pos++;
    if (!is_end() && tokens[pos].type == Token::SEMICOLON) pos++;
    return false;
}

int Parser::cur_line() { return current().line; }
int Parser::prev_line() { return pos > 0 ? tokens[pos - 1].line : 0; }

std::unique_ptr<CompUnit> Parser::parse_compunit(){
    std::unique_ptr<CompUnit> compunit = std::make_unique<CompUnit>();
    compunit->start_line = cur_line();
    while(!is_end()){
        if (current().type == Token::INT || current().type == Token::CHAR || current().type == Token::VOID){
            int start_pos = pos;
            pos++; // skip base type
            while(current().type == Token::AST){
                // don't support function pointer/array pointer for now, so just skip all '*' after type
                pos++;
            }
            if (current().type == Token::ID) {
                pos++;
                if (current().type == Token::LPAREN) {
                    pos = start_pos; // reset to re-parse type and ident in parse_funcdef
                    compunit->add_unit(std::move(parse_funcdef()));
                } else {
                    pos = start_pos; // reset to re-parse type and ident in parse_vardecl
                    compunit->add_unit(std::move(parse_vardecl()));
                }
            } else {
                std::cerr << "Parse error: expect identifier after type" << std::endl;
                return nullptr;
            }
        } else {
            std::cerr << "Parse error: unexpected top-level token " << Token::TypeToString(current().type) << std::endl;
            pos++;
            return nullptr;
        }
    }
    compunit->end_line = prev_line();
    return compunit;
}

std::unique_ptr<FuncDef> Parser::parse_funcdef(){
    int start = cur_line();
    // parse return type (base type + pointer qualifiers)
    std::unique_ptr<Type> ret_type = parse_type();
    while (!is_end() && current().type == Token::AST) {
        auto ptr = std::make_unique<Type>();
        ptr->kind = Typekind::PTR;
        ptr->base = std::move(ret_type);
        ret_type = std::move(ptr);
        pos++;
    }
    // parse function name
    if (current().type != Token::ID) {
        std::cerr << "Parse error: expected function name at line "
                  << current().line << std::endl;
        return nullptr;
    }
    std::string name = current().name;
    pos++;
    // parse ( params ) { body }
    expect(Token::LPAREN, "function parameter list");
    std::unique_ptr<Params> params = parse_params();
    expect(Token::RPAREN, "closing ) for parameters");
    std::unique_ptr<Block> body = parse_block();

    auto f = std::make_unique<FuncDef>(std::move(params), std::move(body));
    f->return_type = *ret_type;
    f->ident = name;
    f->start_line = start;
    f->end_line = prev_line();
    return f;
}

std::unique_ptr<Block> Parser::parse_block(){
    int start = cur_line();
    if (!expect(Token::LBRACE, "block start '{'"))
        return nullptr;
    std::unique_ptr<Block> block = std::make_unique<Block>();
    while(!is_end() && current().type != Token::RBRACE){
        std::unique_ptr<ASTNode> s = parse_stmt();
        if (s) block->add_stmt(std::move(s));
        else break;
    }
    expect(Token::RBRACE, "block end '}'");
    block->start_line = start;
    block->end_line = prev_line();
    return block;
}

std::unique_ptr<Type> Parser::parse_type(){
    std::unique_ptr<Type> t = std::make_unique<Type>();
    if(!is_end()){
        if(current().type == Token::INT){
            t->kind = Typekind::INT;
            pos++;
        } else if(current().type == Token::CHAR){
            t->kind = Typekind::CHAR;
            pos++;
        } else if(current().type == Token::VOID){
            t->kind = Typekind::VOID;
            pos++;
        } else {
            std::cerr << "Parse error: expected type at line " << current().line << std::endl;
        }
    }
    return t;
}

std::unique_ptr<ASTNode> Parser::parse_expr(){

}

std::unique_ptr<ASTNode> Parser::parse_stmt(){

}

std::unique_ptr<Params> Parser::parse_params(){

}

std::unique_ptr<VarDecl> Parser::parse_vardecl(){
    int start = cur_line();
    // parse base type (e.g. int/char/void)
    std::unique_ptr<Type> base_type = parse_type();
    auto vardecl = std::make_unique<VarDecl>(*base_type);

    while (true) {
        // each declarator has its own pointer qualifiers
        Type var_type_copy = *base_type;
        std::unique_ptr<Type> var_type = std::make_unique<Type>(var_type_copy);
        //TODO: support function pointer/array pointer if needed
        while (!is_end() && current().type == Token::AST) {
            auto ptr = std::make_unique<Type>();
            ptr->kind = Typekind::PTR;
            ptr->base = std::move(var_type);
            var_type = std::move(ptr);
            pos++;
        }

        // parse identifier
        if (current().type != Token::ID) {
            std::cerr << "Parse error: expected identifier at line "
                      << current().line << std::endl;
            return nullptr;
        }
        std::string name = current().name;
        pos++;

        // parse array dimensions: [expr][expr]...
        while (!is_end() && current().type == Token::LBRACK) {
            pos++; // skip [
            // TODO: parse constant expression for array size
            int size = 0;
            if (!is_end() && current().type == Token::DEC) {
                size = std::stoi(current().name);
                pos++;
            }
            auto arr = std::make_unique<Type>();
            arr->kind = Typekind::ARR;
            arr->base = std::move(var_type);
            arr->arr_size = size;
            var_type = std::move(arr);
            expect(Token::RBRACK, "closing ] for array");
        }

        // parse optional initializer
        std::unique_ptr<ASTNode> init = nullptr;
        if (!is_end() && current().type == Token::ASSIGN) {
            pos++; // skip =
            if (!is_end() && current().type == Token::LBRACE) {
                init = parse_initlist();
            } else {
                init = parse_expr();
            }
        }

        vardecl->add_def(std::make_unique<VarDef>(name, *var_type, std::move(init)));

        if (!is_end() && current().type == Token::COMMA) {
            pos++; // skip , → next declarator
        } else {
            break;
        }
    }
    expect(Token::SEMICOLON, "end of variable declaration");
    vardecl->start_line = start;
    vardecl->end_line = prev_line();
    return vardecl;
}