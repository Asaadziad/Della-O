#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include "lexer.h"


class Parser {
  std::unique_ptr<Expr> root;  
  public:
    Parser(std::string source);
    void init();
    void advance();
    bool match(TokenType type); // checks if the current token matches the type given
    Token peek_current();
    Token peek_next_token(); 
  private:  
    int                current;
    std::unique_ptr<Lexer> lexer;
};

#endif
