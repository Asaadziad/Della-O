#ifndef COMPILER_H
#define COMPILER_H

#include <unordered_map>
#include "parser.h"

class Compiler {
  public:
    Compiler() {};
    Compiler(std::string file_name){
      Parser parser(file_name);
      root = std::move(parser.parse()); 
      Vars m;
      globals.variables.push_back(m);
      globals.current_scope = 0;
      
    };
    void compile();
  private:
    std::unique_ptr<Expr> root;
    Locals globals;    
};

#endif
