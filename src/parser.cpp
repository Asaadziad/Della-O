#include "parser.h"
#include <iostream>
#include <memory>
#include <optional>
#include "compiler.h"

std::map<std::string, bool> globals;
std::map<std::string, LType> globals_types;
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

static std::unique_ptr<Expr> parse_var(Parser& parser) {
  std::string var_name = getCurrentTokenView(parser);
  parser.advance(); 
  if(getCurrentTokenType(parser) == TOKEN_COLON) {
    auto type = parse_type(parser);
    return std::make_unique<VarExpr>(var_name, type);
  }
  return std::make_unique<VarExpr>(var_name);
}

static std::unique_ptr<Expr> parse_func_call(Parser& parser) {
  std::string fun_name = getCurrentTokenView(parser);
  parser.advance();
  consume("(", parser);
   
  std::vector<std::unique_ptr<Expr>> args;
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
   
  return std::make_unique<FunCall>(std::move(fun_name), std::move(args));
} 


static std::unique_ptr<Expr> parse_primary(Parser& parser) {
  switch(getCurrentTokenType(parser)) { 
    case TOKEN_INTEGER:{ 
      double v = std::stoi(getCurrentTokenView(parser));    
      auto root = std::make_unique<NumberExpr>(std::move(v));
      parser.advance();
      return std::move(root);
                       } 
    case TOKEN_IDENTIFIER: {    
      if(getTokenType(parser.peek_next_token()) == TOKEN_LEFT_PAREN) {
        auto root = parse_func_call(parser);
        return std::move(root);

      } else {
        return parse_var(parser);
      } 
     
                          }
    default:{ 
      PANIC("Unexpected token : %s", getCurrentTokenView(parser).c_str());
            }
  }
  return nullptr;
}



static std::unique_ptr<Expr> parse_binary_mult(Parser& parser) {
  auto lhs = parse_primary(parser);
  if(!lhs) {
     PANIC("Couldn't parse lhs"); 
  }
 switch(getCurrentTokenType(parser)) {
  case TOKEN_DIVIDE:{
    consume("/", parser);
    auto rhs = parse_primary(parser);
    if(!rhs){ 
      PANIC("Couldn't parse rhs");  
    }
    
    return std::make_unique<BinaryExpr>(std::move(lhs),std::move(rhs), BINOP_DIVIDE);
  }
  case TOKEN_MULT:{
    consume("*", parser);
    auto rhs = parse_primary(parser);
    if(!rhs){ 
       PANIC("Couldn't parse rhs");  
    }
    return std::make_unique<BinaryExpr>(std::move(lhs),std::move(rhs), BINOP_MULT);
  }
  default: break;  
 }  
 return std::move(lhs); 
}

static std::unique_ptr<Expr> parse_binary_plus(Parser& parser) {
  auto lhs = parse_binary_mult(parser);
  if(!lhs) {
     PANIC("Couldn't parse lhs"); 
  }
 switch(getCurrentTokenType(parser)) {
  case TOKEN_PLUS:{
    consume("+", parser);
    auto rhs = parse_binary_mult(parser);
    if(!rhs){ 
      PANIC("Couldn't parse rhs");  
    }
    
    return std::make_unique<BinaryExpr>(std::move(lhs),std::move(rhs), BINOP_PLUS);
  }
  case TOKEN_MINUS:{
    consume("-", parser);
    auto rhs = parse_binary_mult(parser);
    if(!rhs){ 
       PANIC("Couldn't parse rhs");  
    }
    return std::make_unique<BinaryExpr>(std::move(lhs),std::move(rhs), BINOP_MINUS);
  }
  default: break;  
 }  
 return std::move(lhs); 
}

static std::unique_ptr<Expr> parse_string(Parser& parser) {
  std::string tmp = getCurrentTokenView(parser);
  
  return std::make_unique<StringExpr>(std::move(tmp));
}

static std::unique_ptr<Expr> parse_expression(Parser& parser) {
  if(getCurrentTokenType(parser) == TOKEN_STRING){
    auto str = parse_string(parser);
    parser.advance();  
    return std::move(str);
  }
  return parse_binary_plus(parser);
}

static std::unique_ptr<Expr> parse_print_stmt(Parser& parser) {
  consume("print", parser); // consume keyword
  consume("(", parser); // its a function call after all
  auto exp = parse_expression(parser); // can only print strings for now 
  
  consume(")", parser);
  consume(";", parser);
  return std::make_unique<PrintStatement>(std::move(exp));
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
       // this sohuld be return statement class

        return std::make_unique<ReturnStatement>(std::move(expr), INT);
      } 
    case TOKEN_PRINT:
    {
      auto print = parse_print_stmt(parser);
      return std::move(print); 
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
  if(getTokenType(parser.peek_current()) == TOKEN_LEFT_BRACKET) {
    
    return VOID;
  }
  
  consume(":", parser); // consume the type declarator
  std::string current_view = getCurrentTokenView(parser);
  parser.advance(); 
  if(current_view.compare("int") == 0) {
    
    return INT;
  } else if(current_view.compare("str") == 0){
    return STRING;
  } else { 
    
    return VOID;
  }
}

static std::unique_ptr<Expr> parse_block(Parser& parser, std::vector<std::string> arg_names){
  consume("{", parser);
  std::vector<std::unique_ptr<Expr>> dcls;
  std::map<std::unique_ptr<Expr>, bool> locals;
  while(getCurrentTokenType(parser) != TOKEN_RIGHT_BRACKET && getCurrentTokenType(parser) != TOKEN_RETURN) {
    
    auto root = parse_declaration(parser);
    if(!root) {
      PANIC("Couldn't parse declaration inside function");
    }
 
    dcls.push_back(std::move(root));
  }
  
  
  return std::make_unique<Block>(std::move(dcls));
}



static std::unique_ptr<Expr> parse_func_declaration(Parser& parser) {
  consume("func", parser);
  
  std::string fun_name = getCurrentTokenView(parser);
  globals[fun_name] = 1;
  parser.advance();
  std::vector<std::unique_ptr<VarExpr>> args;
  std::vector<std::string> arg_names;
  if(expect("(", parser)) {
    parser.advance(); 
    while(getCurrentTokenType(parser) != TOKEN_RIGHT_PAREN) { 
      auto var_name = getCurrentTokenView(parser);
      parser.advance();
      arg_names.push_back(var_name);
      auto var_type = parse_type(parser);
      auto root = std::make_unique<VarExpr>(std::move(var_name), var_type);
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
  globals_types[fun_name] = type;
  auto block = parse_block(parser, arg_names); 
  if(type != VOID) {
    consume("return", parser);
    auto return_stmt_e = parse_expression(parser);
    consume(";", parser);
    auto return_stmt = std::make_unique<ReturnStatement>(std::move(return_stmt_e), type);
    consume("}", parser); // cleanup later
    return std::make_unique<FunDeclaration>(std::move(fun_name), std::move(args), std::move(block),std::move(return_stmt) ,type);
  } else {
    consume("}", parser);
    return std::make_unique<FunDeclaration>(std::move(fun_name), std::move(args), std::move(block),nullptr ,type); 
  } 
                          
                        
  
}

static std::unique_ptr<Expr> parse_var_declaration(Parser& parser) {
  consume("let", parser);
 
  
  auto var = parse_expression(parser); 

  consume("=", parser);
  auto exp = parse_statement(parser);
  if(!exp) {
    PANIC("Couldn't parse variable statement");
  }
  
  // return variable declaration
  return std::make_unique<VarDeclaration>(std::move(var), std::move(exp));
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
  FILE* out = fopen("main.ssa", "w+");
  if(!out) exit(1);
  fprintf(out, "data $fmt_int = {b \"%%d\", b 0}\n");
  auto root = parse_program(*this);   
  int stack_size = 0;
  root->generateCode(out, &stack_size);
  fclose(out); 
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
