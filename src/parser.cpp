
#include "parser.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>

Parser::Parser(std::string source) {
  Lexer     lexer(source);
  std::vector<Token> tokenss = lexer.tokenize();
  for(int i = 0; i < tokenss.size(); i++) {
    tokens.push_back(makeToken(getTokenLiteral(tokenss[i]), getTokenType(tokenss[i])));
  }
  current = 0; 
}

typedef enum {
   Expr_Number,
   Expr_BinOp,
   Expr_FunCall,
   Expr_FunDec,
} ExprType;

typedef enum {
  BINOP_PLUS,
  BINOP_MULT,
} BinopType;


class Expr {
  public:
  virtual ~Expr() = default; 
  virtual void generateCode();
      
};

class ExprNumber: public Expr {
  private:
    double val;
  public:
    ExprNumber(double val): val(val){};
};

class ExprVariable: public Expr {
  std::string name;
  public:
    ExprVariable(std::string name):name(name) {};
};

class ExprBinary: public Expr {
  BinopType op_type;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
  public:
    ExprBinary(BinopType op_type, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS):
                op_type(op_type), LHS(std::move(LHS)), RHS(std::move(RHS)) {};
};

class ExprFunCall : public Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  public:
  ExprFunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name),args(std::move(args)) {};
};

class ExprProto {
  std::string name;
  std::vector<std::string> args;
  public:
  ExprProto(std::string name, std::vector<std::string> args): name(name), args(std::move(args)) {};
  const std::string& getName() { return name; }; 
};

class ExprFunDec {
  std::unique_ptr<ExprProto> proto;
  std::unique_ptr<Expr>      body;
  public:
  ExprFunDec(std::unique_ptr<ExprProto> proto, std::unique_ptr<Expr> body): proto(std::move(proto)), body(std::move(body)){};
};

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
  FILE* out = std::fopen("./main.ssa", "wb");
  if(!out) return;
  
  fprintf(out , "export function w $main() {\n");
  fprintf(out , "@start\n");
  
  size_t stack_size = 0;
  while(current < tokens.size()) {  
    Expr* root =  parse_expression(*this);  
    if(!root) {
      std::cout << "root is null" << std::endl;
      return;
    }
    compile_expr(out, root, &stack_size);
  } 
  fprintf(out, "  ret 0\n");
  fprintf(out, "}\n");
  fprintf(out, "data $fmt = { b \"%%d\\n\", b 0}");

  std::fclose(out); 
} 


