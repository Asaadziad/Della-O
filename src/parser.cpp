
#include "parser.h"
#include <iostream>

Parser::Parser(std::string source) {
  Lexer     lexer(source);
  std::vector<Token> tokenss = lexer.tokenize();
  for(int i = 0; i < tokenss.size(); i++) {
    tokens.push_back(makeToken(getTokenLiteral(tokenss[i]), getTokenType(tokenss[i])));
  }
  current = 0; 
}


static void consumeToken(std::vector<Token>& tokens) {
  #ifdef DEBUG_FLAG
    printToken(tokens[0]);
  #endif
  tokens.erase(tokens.begin());
}

bool Parser::match(TokenType type) {
  Token current = peek_current();
  advance();
  if(getTokenType(current) == type) return true;
  return false;
}

typedef struct expr *Expr;

typedef struct expr_binop {
  char op;
  Expr lhs;
  Expr rhs;
} *ExprBin;

typedef struct expr_funcall {} *ExprFunCall;

struct expr {
  
};

// parses numbers and funcalls
Expr parse_primary() {}

Expr parse_binop_expr() {}

Expr parse_expression() {}

Token Parser::peek_current() {
  if(current >= tokens.size()) return NULL;
  return tokens[current];
}

Token Parser::peek_next_token() {
  if(current + 1 >= tokens.size()) return NULL;

  return tokens[current + 1];
}

void Parser::advance() {
  if(current >= tokens.size()) return;
  current++;
}

void Parser::init(){ 
  while(current < tokens.size()) {
    Token previous = peek_current();
    advance();
    TokenType type = getTokenType(previous);
    
    if(type == TOKEN_LEFT_PAREN) {
      Token current_token = peek_current();
      type = getTokenType(current_token);
      
      while(type != TOKEN_RIGHT_PAREN) {
        #ifdef DEBUG_FLAG 
        printToken(current_token);
        #endif
        advance(); 
        current_token = peek_current();
        if(current_token == NULL) break;
        type = getTokenType(current_token);
        
      }
      
    }
      
  } 

}
