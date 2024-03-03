
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


typedef enum {
   Expr_Number,
   Expr_BinOp,
   Expr_FunCall,
} ExprType;

typedef enum {
  BINOP_PLUS,
  BINOP_MULT,
} BinopType;


class Expr {
  public:
  Expr(){};
  Expr(int number): number(number) {};
  ~Expr(); 
  int value() {
    return number;
  };
  ExprType getType() {
    return expr_type;
  };
  virtual Expr* getLhs();
  virtual Expr* getRhs();
  private:
    ExprType expr_type;
    int number;
};

class ExprBinary: public Expr {
  public:
    ExprBinary(Expr* lhs,Expr* rhs,BinopType binop): lhs(lhs), rhs(rhs), binop(binop) {};
    ~ExprBinary() {
      delete lhs;
      delete rhs;
    };
    Expr* getLhs() {
      return lhs;
    };
    Expr* getRhs() {
      return rhs;
    };
  private:
    Expr*     lhs;
    Expr*     rhs;
    BinopType binop;
};

class ExprFunCall: public Expr {
  public:
    ExprFunCall(){};
    ~ExprFunCall() {
      for(auto expr: args) {
        delete expr;
      }
    };
  private:
    std::vector<Expr*> args;
    std::string        name;
};



// parses numbers and funcalls
Expr* parse_primary(Parser& parser) {
  Token t = parser.peek_current();
  parser.advance();
  if(getTokenType(t) == TOKEN_INTEGER) {
    
    int num = std::stoi(getTokenLiteral(t));
    Expr* number = new Expr((int)num);   
    return number;
  } else {
    std::cout << "unexpected token" << std::endl;
  }
  return NULL;
}

Expr* parse_expr_mult(Parser& parser) 
{
  Expr* lhs = parse_primary(parser);
  if(!lhs) return NULL; 
  if(getTokenType(parser.peek_next_token()) == TOKEN_PLUS) {
    parser.advance();
    Expr* rhs = parse_primary(parser);
    if(!rhs) return NULL;
    Expr* plus = new ExprBinary(lhs, rhs, BINOP_MULT);  
    if(!plus) return NULL;
    return plus;
  }
  return lhs;

}

Expr* parse_expr_plus(Parser& parser) {
  Expr* lhs = parse_expr_mult(parser);
  if(!lhs) {
    std::cout << "im at expr_plus lhs" << std::endl;
    return NULL; }
  if(getTokenType(parser.peek_next_token()) == TOKEN_PLUS) {
    parser.advance();
    Expr* rhs = parse_expr_mult(parser);
    if(!rhs) {
      std::cout << "im at expr_plus rhs" << std::endl;
      return NULL;
    }
    Expr* plus = new ExprBinary(lhs, rhs, BINOP_PLUS); 
    if(!plus) {
      std::cout << "im at expr_plus plus" << std::endl;
      return NULL;
    } 
    return plus;
  }
  return lhs;
}

Expr* parse_expression(Parser& parser) {
 return parse_expr_plus(parser); 
}

void compile_expr(Expr* expr){
  switch(expr->getType()) {
    case Expr_Number:
      std::cout << expr->value();
      break;
    case Expr_BinOp:
      
      compile_expr(expr->getLhs());
      compile_expr(expr->getRhs()); 
    break;
    case Expr_FunCall:
    break;
     
  };
}

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
    Expr* root =  parse_expression(*this);  
    if(!root) {
      std::cout << "root is null" << std::endl;
      return;
    }
    compile_expr(root);
  }
      
} 


