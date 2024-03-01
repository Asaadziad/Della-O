#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "lexer.h"

typedef struct ast_node_t {
  struct ast_node_t*  left;
  Token               op;
  struct ast_node_t*  right; 
} *AstNode;

typedef struct ast_t {
  AstNode root;
} *AST;

class Parser {
  public:
    Parser(std::string source);
    void init();
    void advance();
    bool match(TokenType type); // checks if the current token matches the type given
    Token peek_current();
    Token peek_next_token(); 
  private: 
    std::vector<Token> tokens;
    int                current;
};

#endif
