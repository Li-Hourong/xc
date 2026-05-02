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

std::unique_ptr<CompUnit> Parser::parse_compunit(){
    std::unique_ptr<CompUnit> compunit = std::make_unique<CompUnit>();
    while(!is_end()){
        if (current().type == Token::INT || current().type == Token::CHAR || current().type == Token::VOID){
            std::unique_ptr<Type> t = parse_type();
            if (current().type == Token::ID) {
                std::string name = current().name;
                pos++;
                if (current().type == Token::LPAREN) { //FuncDef
                    compunit->add_unit(std::move(parse_funcdef(*t, name)));
                } else {
                    compunit->add_unit(std::move(parse_vardecl(*t, name)));
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
    return compunit;
}

std::unique_ptr<FuncDef> Parser::parse_funcdef(Type t, std::string ident){
    if (!expect(Token::LPAREN, "function parameter list")) return nullptr;
    std::unique_ptr<Params> params = parse_params();
    expect(Token::RPAREN, "closing ) for parameters");
    std::unique_ptr<Block> body = parse_block();
    std::unique_ptr<FuncDef> f = std::make_unique<FuncDef>(std::move(params), std::move(body));
    f->return_type = t;
    f->ident = ident;
    return f;
}

std::unique_ptr<Block> Parser::parse_block(){
    if (!expect(Token::LBRACE, "block start '{'")) 
        return nullptr;
    std::unique_ptr<Block> block = std::make_unique<Block>();
    while(!is_end() && current().type != Token::RBRACE){
        std::unique_ptr<ASTNode> s = parse_stmt();
        if (s) block->add_stmt(std::move(s));
        else break;
    }
    expect(Token::RBRACE, "block end '}'");
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

std::unique_ptr<VarDecl> Parser::parse_vardecl(Type t, std::string first_name){
    std::unique_ptr<VarDecl> decl = std::make_unique<VarDecl>(t);
    decl->add_def(std::make_unique<VarDef>(first_name, t));
    while(current().type == Token::COMMA){
        pos++;
        if(current().type == Token::ID){
            std::string next_name = current().name;
            pos++;
            decl->add_def(std::make_unique<VarDef>(next_name, t));
        } else {
            std::cerr << "Parse error: expected identifier after ',' in variable declaration" << std::endl;
            break;
        }
    }
    expect(Token::SEMICOLON, "end of variable declaration");
    return decl;
}