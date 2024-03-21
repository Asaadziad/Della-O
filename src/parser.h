#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include "lexer.h"
#include "expressions.h"

class Parser { 
   
  public:
    Parser(std::string source);
    void init();
    void advance(); 
    Token peek_current();
    Token peek_next_token(); 
    inline std::unique_ptr<Expr>& getAst() { return ast_root;};
    
    int get_current_i() {return current;};
    int get_max_i() { return lexer->tokenize().size();};
  private:  
    int                    current;
    std::unique_ptr<Lexer> lexer;
    std::unique_ptr<Expr>  ast_root;
};

#endif
