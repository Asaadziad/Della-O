#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include "lexer.h"
#include "expressions.h"
#include <unordered_map>

class Parser {   
  public:
    Parser(std::string source);
    std::unique_ptr<Expr> parse(); // returns root of AST
    void advance(); 
    Token peek_current();
    Token peek_next_token();         
    int get_current_i() {return current;};
    int get_max_i() { return lexer->tokenize().size();};
  private:  
    int                    current;
    std::unique_ptr<Lexer> lexer;
    
};

#endif
