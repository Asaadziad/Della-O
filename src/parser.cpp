
#include "parser.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>


Parser::Parser(std::string source) {
  Lexer     lexer(source);
  std::vector<Token> tokenss = lexer.tokenize();
  for(int i = 0; i < tokenss.size(); i++) {
    auto token =  makeToken(getTokenLiteral(tokenss[i]), getTokenType(tokenss[i]));  
    tokens.push_back(token);
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
  virtual void generateCode(FILE* out,int* stack_size) = 0;
      
};

class ExprNumber: public Expr {
  private:
    double val;
  public:
    ExprNumber(double val): val(val){};
    virtual void generateCode(FILE* out, int* stack_size) override {
     fprintf(out, "   %%s%d =w copy %d\n", *stack_size, (int)val);    
     *stack_size += 1;
    }
};

class ExprVariable: public Expr {
  std::string name;
  public:
    ExprVariable(std::string name):name(name) {};
    virtual void generateCode(FILE* out,int* stack_size) override {
      std::cout << "Generated Var" << std::endl;
      *stack_size += 1;
    }
};

class ExprBinary: public Expr {
  BinopType op_type;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
  public:
    ExprBinary(BinopType op_type, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS):
                op_type(op_type), LHS(std::move(LHS)), RHS(std::move(RHS)) {};
    virtual void generateCode(FILE* out,int* stack_size) override {
      LHS->generateCode(out, stack_size);
      RHS->generateCode(out, stack_size);
      switch(op_type) {
        case BINOP_PLUS:
          fprintf(out,  "   %%s%d =w add %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size - 1);
          *stack_size -= 1;
          break;
        case BINOP_MULT:
          fprintf(out, "   %%s%d =w mul %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size - 1);
          *stack_size -= 1;
          break; 
      }
       
    }

};

class ExprFunCall : public Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  public:
  ExprFunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name),args(std::move(args)) {};
  virtual void generateCode(FILE* out,int* stack_size) override {
    int tmp = *stack_size;
    for(auto& argument: args) {
      argument->generateCode(out, stack_size);
    }
    fprintf(out, "  call $%s(" , name.c_str());
    while(*stack_size > tmp) {
      fprintf(out, "%%s%d, ", *stack_size - 1);
      *stack_size -= 1;
    } 
    fprintf(out, ")\n");
  }
};

class ExprProto {
  std::string name;
  std::vector<std::string> args;
  public:
  ExprProto(std::string name, std::vector<std::string> args): name(name), args(std::move(args)) {};
  const std::string& getName() { return name; }; 
  const std::vector<std::string> getArgs() { return args; };
};

class ExprFunDec {
  std::unique_ptr<ExprProto> proto;
  std::unique_ptr<Expr>      body;
  public:
  ExprFunDec(std::unique_ptr<ExprProto> proto, std::unique_ptr<Expr> body): 
    proto(std::move(proto)), body(std::move(body)){};
  void generateCode(FILE* out) {
    fprintf(out, "function w $%s(", proto->getName().c_str());
    for(auto& argName : proto->getArgs()) {
      fprintf(out, " %s,", argName.c_str());
    }
    fprintf(out, "){\n@start\n");
    int stack_size = 0;
    body->generateCode(out, &stack_size);
    fprintf(out, "  ret %%s%d\n", stack_size - 1);
    fprintf(out, "}\n"); 
  };
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
  if(getTokenType(parser.peek_current()) != TOKEN_SEMICOLON) {
    std::cerr << "Syntax error: Expected ;" << std::endl;
    return nullptr;
  }
  parser.advance();
  return std::make_unique<ExprFunCall>(id_name, std::move(args));
}

// paren expression := "(" + expression + ")"
static std::unique_ptr<Expr> parseParen(Parser& parser) {
  
  parser.advance(); // eat  ( current token
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

// block := '{' + expression + '}'
static std::unique_ptr<Expr> parseBlock(Parser& parser) {
  parser.advance(); // eat {
  auto expression = parseExpression(parser);
  if(!expression) return nullptr;
  if(getTokenType(parser.peek_current()) != TOKEN_RIGHT_BRACKET) {
    
    std::cerr << "Expected }" << std::endl;
    return nullptr; 
  } 
  parser.advance(); // eat }
  return expression;
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
    case TOKEN_LEFT_BRACKET:
    return parseBlock(parser); 
    default: 
    std::cerr << "Unexpected token" << std::endl;
    return nullptr;
  }
}

static std::unique_ptr<Expr> parseMul(Parser& parser) {
  auto LHS = parsePrimary(parser);
  
  TokenType type = getTokenType(parser.peek_current()); 
  switch(type) {
    case TOKEN_PLUS: {
      parser.advance();                       
      auto RHS = parseExpression(parser);
      return std::make_unique<ExprBinary>(BINOP_PLUS, std::move(LHS), std::move(RHS));
                     }
    case TOKEN_MULT:{
      parser.advance();
      auto RHS = parseExpression(parser);
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

static std::unique_ptr<ExprProto> parseProtoType(Parser& parser){
    if(getTokenType(parser.peek_current()) != TOKEN_IDENTIFIER){
      std::cerr << "Expected function name" << std::endl;
      return nullptr;
    }

    std::string fName = getTokenLiteral(parser.peek_current());
    parser.advance();

    if(getTokenType(parser.peek_current()) != TOKEN_LEFT_PAREN) {
      std::cerr << "Expected '('" << std::endl;
      return nullptr;
    }
    parser.advance(); // eat (
                      
    std::vector<std::string> argNames;
    while(getTokenType(parser.peek_current()) == TOKEN_IDENTIFIER) {
      argNames.push_back(getTokenLiteral(parser.peek_current()));
      parser.advance();
      if(getTokenType(parser.peek_current()) == TOKEN_RIGHT_PAREN) break;
      if(getTokenType(parser.peek_current()) != TOKEN_COLON) {
        std::cerr << "Expected , "<< std::endl;
        return nullptr;
      }
      parser.advance();
    }

    parser.advance(); // eat )


    return std::make_unique<ExprProto>(fName, std::move(argNames));
}

static std::unique_ptr<ExprFunDec> parseDec(Parser& parser) {
  parser.advance(); // eat 'function' keyword
  auto proto = parseProtoType(parser);
  if(!proto) {
    return nullptr;
  } 
  parser.advance(); // eat {

  auto e = parseExpression(parser);
  if(!e) { return nullptr; }
  parser.advance(); // eat }
  return std::make_unique<ExprFunDec>(std::move(proto), std::move(e));
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
   
  int stack_size = 0;
  while(current < tokens.size()) {   
   if(getTokenType(peek_current()) == TOKEN_FUNDEC){
       auto root = parseDec(*this);
        if(!root) {
          std::cout << "root is null fun dec" << std::endl;
          return;
        } 
       root->generateCode(out); 
   } else {
       auto root =  parseExpression(*this);  
       if(!root) {
         std::cout << "root is null" << std::endl;
         return;
       }
       root->generateCode(out, &stack_size);  
   }
  } 

  std::fclose(out); 
} 


