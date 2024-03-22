#include "parser.h"
#include <iostream>
#include <memory>
#include <optional>



Parser::Parser(std::string source): lexer(std::make_unique<Lexer>(source)), current(0) {
  lexer->init();
}

static std::string getCurrentTokenView(Parser& parser) {
  return getTokenLiteral(parser.peek_current());
}

static TokenType getCurrentTokenType(Parser& parser) {
  return getTokenType(parser.peek_current());
}

static void panic(const char* msg,const char* file,int line, ...) {
  
  fprintf(stderr, "[%s, %d]: ",  file, line);
  va_list args;
  va_start(args, line);
  vfprintf(stderr,msg, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(1);
}
#define PANIC(msg, ...) panic(msg,__FILE__, __LINE__, ##__VA_ARGS__ ) 

static bool expect(std::string token_view, Parser& parser) {
  if(token_view.compare(getCurrentTokenView(parser)) == 0) {
    return true;
  }
  return false;
}

static void consume(std::string token_view, Parser& parser) {
  
  if(expect(token_view, parser)) {
    parser.advance();
  } else {  
   PANIC("Syntax error: expected '%s', found instead: '%s' ", token_view.c_str(), getCurrentTokenView(parser).c_str());
  } 
}

/*
 * Function Declarations
 * */
static std::unique_ptr<Expr> parse_expression(Parser& parser);
static std::unique_ptr<Expr> parse_declaration(Parser& parser);
static LType parse_type(Parser& parser);
/*
 * End
 * */
static std::unique_ptr<Expr> parse_primary(Parser& parser) {
  switch(getCurrentTokenType(parser)) { 
    case TOKEN_INTEGER:{ 
      
         double v = std::stoi(getCurrentTokenView(parser));    
         auto root = std::make_unique<NumberExpr>(std::move(v));
      
      parser.advance();
      return std::move(root);
                       } 
    case TOKEN_IDENTIFIER: {
      std::string var_name = getCurrentTokenView(parser);
      parser.advance(); 
      auto type = parse_type(parser); 
      auto root = std::make_unique<VarExpr>(var_name, type);
      
      
      return std::move(root);
                          }
    default:{ 
      PANIC("Unexpected token : %s", getCurrentTokenView(parser).c_str());
            }
  }
  return nullptr;
}


static std::unique_ptr<Expr> parse_binary(Parser& parser) {
  auto lhs = parse_primary(parser);
  if(!lhs) {
     PANIC("Couldn't parse lhs"); 
  }
 switch(getCurrentTokenType(parser)) {
  case TOKEN_PLUS:{
    consume("+", parser);
    auto rhs = parse_expression(parser);
    if(!rhs){ 
      PANIC("Couldn't parse rhs");  
    }
    
    return std::make_unique<BinaryExpr>(std::move(lhs),std::move(rhs), BINOP_PLUS);
  }
  case TOKEN_MULT:{
    consume("*", parser);
    auto rhs = parse_expression(parser);
    if(!rhs){ 
       PANIC("Couldn't parse rhs");  
    }
    return std::make_unique<BinaryExpr>(std::move(lhs),std::move(rhs), BINOP_MULT);
  }
  default: break;  
 }  
 return std::move(lhs); 
}

static std::unique_ptr<Expr> parse_expression(Parser& parser) {
  return parse_binary(parser);
}



// statement := expression;
static std::unique_ptr<Expr> parse_statement(Parser& parser) {
  TokenType type = getCurrentTokenType(parser);
  switch(type) {
    case TOKEN_RETURN:
      {
        consume("return", parser);
        auto expr = parse_expression(parser);        
        consume(";", parser);
        return std::move(expr);
      }
    default:
      {
        auto expr = parse_expression(parser);
        consume(";", parser);
        return std::move(expr);            
      };
  } 
}

static LType parse_type(Parser& parser) {
  consume(":", parser); // consume the type declarator
  std::string current_view = getCurrentTokenView(parser);
  parser.advance(); 
  if(current_view.compare("int") == 0) {
    
    return INT;
  } else { 
    
    return VOID;
  }
}

static std::unique_ptr<Expr> parse_func_declaration(Parser& parser) {
  consume("func", parser);
  
  std::string fun_name = getCurrentTokenView(parser);
  parser.advance();
  std::vector<std::unique_ptr<Expr>> args;
  if(expect("(", parser)) {
    parser.advance(); 
    while(getCurrentTokenType(parser) != TOKEN_RIGHT_PAREN) { 
      auto root = parse_expression(parser);
      if(!root) {
        PANIC("Couldn't parse argument");
      }
      if(getCurrentTokenType(parser) == TOKEN_COMMA) { 
        parser.advance();
      } 
      args.push_back(std::move(root));
      
    }
    consume(")", parser);
  } 
  
  
  auto type = parse_type(parser);
  consume("{", parser);
  std::vector<std::unique_ptr<Expr>> dcls;
  while(getCurrentTokenType(parser) != TOKEN_RIGHT_BRACKET) {
    auto root = parse_declaration(parser);
    if(!root) {
      PANIC("Couldn't parse declaration inside function");
    }
    dcls.push_back(std::move(root));
  }
  consume("}", parser);

  return std::make_unique<FunDeclaration>(std::move(fun_name), std::move(args), std::move(dcls), type);
}

static std::unique_ptr<Expr> parse_var_declaration(Parser& parser) {
  consume("let", parser);
  std::vector<std::unique_ptr<Expr>> stmts;
  auto var = parse_expression(parser);

  stmts.push_back(std::move(var));
  
  consume("=", parser);
  auto exp = parse_statement(parser);
  if(!exp) {
    PANIC("Couldn't parse variable statement");
  }
  stmts.push_back(std::move(exp));
  // return variable declaration
  return std::make_unique<Declaration>(std::move(stmts), EXPR_VARDEC);
}

// funDeclaration | varDeclaration | statement
static std::unique_ptr<Expr> parse_declaration(Parser& parser) {
  if(getCurrentTokenType(parser) == TOKEN_LET) {
    return parse_var_declaration(parser);
  } else if(getCurrentTokenType(parser) == TOKEN_FUN) {
    return parse_func_declaration(parser);
  }  else {
    return parse_statement(parser);
  }
}

static std::unique_ptr<Expr> parse_program(Parser& parser) {
  std::vector<std::unique_ptr<Expr>> dcls;
 while(parser.get_current_i() < parser.get_max_i()) {
    auto root = parse_declaration(parser); 
    dcls.push_back(std::move(root)); 
 } 
 return std::make_unique<Program>(std::move(dcls)); 
}

void Parser::init(){  
  ast_root = parse_program(*this);  
  ast_root->generateCode(); 
} 

Token Parser::peek_current() {
  if(current >= lexer->tokenize().size()) return NULL;
  return lexer->tokenize()[current];
}

Token Parser::peek_next_token() {
  if(current + 1 >= lexer->tokenize().size()) return NULL;

  return lexer->tokenize()[current + 1];
}

void Parser::advance() {
  if(current >= lexer->tokenize().size()) return;
  current++;
}
