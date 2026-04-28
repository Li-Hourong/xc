#include "parser.hpp"
#include <memory>

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

std::unique_ptr<CompUnit> parse(std::vector<Token>& tokens)
{

}