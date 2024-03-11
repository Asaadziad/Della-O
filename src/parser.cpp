
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
  virtual void generateCode() = 0;
      
};

class ExprNumber: public Expr {
  private:
    double val;
  public:
    ExprNumber(double val): val(val){};
    virtual void generateCode() override {
      std::cout << "Generated number : " << val << std::endl;
    }
};

class ExprVariable: public Expr {
  std::string name;
  public:
    ExprVariable(std::string name):name(name) {};
    virtual void generateCode() override {
      std::cout << "Generated Var" << std::endl;
    }
};

class ExprBinary: public Expr {
  BinopType op_type;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
  public:
    ExprBinary(BinopType op_type, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS):
                op_type(op_type), LHS(std::move(LHS)), RHS(std::move(RHS)) {};
    virtual void generateCode() override {
      LHS->generateCode();
      std::cout << op_type << std::endl;
      RHS->generateCode();
    }

};

class ExprFunCall : public Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  public:
  ExprFunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name),args(std::move(args)) {};
  virtual void generateCode() override {
    std::cout << "generated funcall" << std::endl;
  }
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
  ExprFunDec(std::unique_ptr<ExprProto> proto, std::unique_ptr<Expr> body): 
    proto(std::move(proto)), body(std::move(body)){};
};

/*
 * Function declaration
 * */

static std::unique_ptr<Expr> parseExpression(Parser& parser);

/*
 * End of function declaration
 * */


static std::unique_ptr<Expr> parseNumber(Parser& parser) {
  Token current_number = parser.peek_current();
  int token_val = std::stoi(getTokenLiteral(current_number));
  auto num_expr = std::make_unique<ExprNumber>(token_val);
  parser.advance();
  return std::move(num_expr);
}

static std::unique_ptr<Expr> parseIdentifier(Parser& parser) {
  std::string id_name = getTokenLiteral(parser.peek_current());
  parser.advance();
  if(getTokenType(parser.peek_current()) != TOKEN_LEFT_PAREN) {
    return std::make_unique<ExprVariable>(id_name);
  }

  parser.advance();
  std::vector<std::unique_ptr<Expr>> args;
  if(getTokenType(parser.peek_current()) != TOKEN_RIGHT_PAREN) {
    while(true) {
      auto arg = parseExpression(parser);
      
      if(arg) {
        args.push_back(std::move(arg));
      } else {
        return nullptr;
      }  

      if(getTokenType(parser.peek_current()) == TOKEN_RIGHT_PAREN){
        break;
      }
      
      if(getTokenType(parser.peek_current()) != TOKEN_COLON) {
        std::cerr << "Expected , " << std::endl;
        return nullptr;
      }
      parser.advance();
   }
  }
  parser.advance(); // eat )
  return std::make_unique<ExprFunCall>(id_name, std::move(args));
}

// paren expression := "(" + expression + ")"
static std::unique_ptr<Expr> parseParen(Parser& parser) {
  parser.advance(); // eat current token
  auto expression = parseExpression(parser);
  if(!expression) {
    return nullptr;
  }

  // check if current token is ")"
  if(getTokenType(parser.peek_current()) != TOKEN_RIGHT_PAREN) {
    std::cerr << "Expected )" << std::endl;
    return nullptr; 
  }

  //eat Token
  parser.advance();
  return expression;
}

Token Parser::peek_current() {
  if(current >= tokens.size()) return NULL;
  return tokens[current];
}

static std::unique_ptr<Expr> parsePrimary(Parser& parser) {
  TokenType type = getTokenType(parser.peek_current());
  switch(type) {
    case TOKEN_IDENTIFIER:
    return parseIdentifier(parser);
    case TOKEN_INTEGER:
    return parseNumber(parser);
    case TOKEN_LEFT_PAREN:
    return parseParen(parser);
    default: 
    std::cerr << "Unexpected token" << std::endl;
    return nullptr;
  }
}

static std::unique_ptr<Expr> parseMul(Parser& parser) {
  auto LHS = parsePrimary(parser);
  
  TokenType type = getTokenType(parser.peek_current());
  parser.advance();
  switch(type) {
    case TOKEN_PLUS: {
      auto RHS = parseMul(parser);
      return std::make_unique<ExprBinary>(BINOP_PLUS, std::move(LHS), std::move(RHS));
                     }
    case TOKEN_MULT:{
      auto RHS = parseMul(parser);
      return std::make_unique<ExprBinary>(BINOP_MULT, std::move(LHS), std::move(RHS));
                    }
    default: 
       
      break;
  }

  return LHS;
}

static std::unique_ptr<Expr> parseExpression(Parser& parser) {
  return parseMul(parser);
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
    auto root =  parseExpression(*this);  
    if(!root) {
      std::cout << "root is null" << std::endl;
      return;
    }
   root->generateCode();  
  } 
  fprintf(out, "  ret 0\n");
  fprintf(out, "}\n");
  fprintf(out, "data $fmt = { b \"%%d\\n\", b 0}");

  std::fclose(out); 
} 


