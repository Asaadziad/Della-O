#include "parser.h"
#include <iostream>
#include <memory>
#include <optional>
#include "compiler.h"

Parser::Parser(std::string source): lexer(std::make_unique<Lexer>(source)), current(0) {
  lexer->init();
}

static std::string getCurrentTokenView(Parser& parser) {
  return getTokenLiteral(parser.peek_current());
}

static TokenType getCurrentTokenType(Parser& parser) {
  return getTokenType(parser.peek_current());
}

static bool expect(std::string token_view, Parser& parser) {
  if(token_view.compare(getCurrentTokenView(parser)) == 0) {
    return true;
  }
  return false;
}

static void consume(std::string token_view, Parser& parser) {
  if(expect(token_view, parser)) {
    parser.advance();
  } 
}

/*
 * Function declaration
 * */

static std::unique_ptr<Expr> parseExpression(Parser& parser);
static std::unique_ptr<Expr> parseIdentifier(Parser& parser);
static std::unique_ptr<Expr> parseDeclaration(Parser& parser);

/*
 * End of function declaration
 * */


static std::unique_ptr<Expr> parseNumber(Parser& parser) { 
  int token_val = std::stoi(getCurrentTokenView(parser));
  auto num_expr = std::make_unique<ExprNumber>(token_val);
  parser.advance();
  return std::move(num_expr);
}

static bool parseArgsExprs(Parser& parser, std::vector<std::unique_ptr<Expr>>& args) {
  parser.advance(); // eat ( 
  if(getTokenType(parser.peek_current()) != TOKEN_RIGHT_PAREN) {
    while(true) {
      auto arg = parseExpression(parser);
      
      if(arg) {
        args.push_back(std::move(arg));
      } else {
        return false;
      }  

      if(getTokenType(parser.peek_current()) == TOKEN_RIGHT_PAREN){
        break;
      }
      
      if(getTokenType(parser.peek_current()) != TOKEN_COLON) {
        std::cerr << "Expected , " << std::endl;
        return false;
      }
      parser.advance();
   }
  }
  parser.advance(); // eat )
  return true;
}

static std::unique_ptr<Expr> parseIdentifier(Parser& parser) { 
  
  std::string id_name = getTokenLiteral(parser.peek_current());
  parser.advance();
  if(getCurrentTokenType(parser) != TOKEN_LEFT_PAREN) { 
   return std::make_unique<ExprVariable>(id_name);
    
  } 
  std::vector<std::unique_ptr<Expr>> args;
  if(!parseArgsExprs(parser,args)) {
    std::cerr << "Couldn't parse Arguments" << std::endl;
    return nullptr;
  };
  
  
  consume(";", parser); 
  return std::make_unique<ExprFunCall>(id_name, std::move(args));
}

// paren expression := "(" + expression + ")"
static std::unique_ptr<Expr> parseParen(Parser& parser) {
  
  consume("(", parser); 
  auto expression = parseExpression(parser);
  if(!expression) { 
    return nullptr;
  }

  // check if current token is ")"
  if(getTokenType(parser.peek_current()) != TOKEN_RIGHT_PAREN) {
    
    std::cerr << "Expected )" << std::endl;
    return nullptr; 
  } 
  //eat ')' Token
  consume(")", parser); 
  return expression;
}


static std::unique_ptr<Expr> parsePrimary(Parser& parser) {
  TokenType type = getCurrentTokenType(parser); 
  switch(type) {    
    case TOKEN_IDENTIFIER:
    return parseIdentifier(parser);
    case TOKEN_INTEGER:
    return parseNumber(parser);
    case TOKEN_LEFT_PAREN: 
    return parseParen(parser); 
    default:
    std::cout << getCurrentTokenView(parser); 
    std::cerr << "Unexpected token" << std::endl;
    return nullptr;
  }
}

static std::unique_ptr<Expr> parseMul(Parser& parser) { 
  auto LHS = parsePrimary(parser);
  TokenType type = getCurrentTokenType(parser);
  switch(type) {
    case TOKEN_PLUS: {
      consume("+", parser);       
      auto RHS = parseExpression(parser);
      return std::make_unique<ExprBinary>(BINOP_PLUS, std::move(LHS), std::move(RHS));
                     }
    case TOKEN_MULT:{
      consume("*", parser);
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

static std::unique_ptr<Expr> parseReturnStatement(Parser& parser) {
  consume("return", parser); 
  auto root = parseExpression(parser);
  if(!root) {
    std::cerr << "Couldnt parse expression" << std::endl;
    return nullptr;
  }
  consume(";", parser); 
  
  return std::make_unique<RetStmt>(std::move(root));
}

static std::unique_ptr<Expr> parseExprStatement(Parser& parser) { 
  auto root = parseExpression(parser);
    if(!root) {
      std::cerr << "Root is null expr statement" << std::endl;
      return nullptr;
    }
  consume(";", parser);
  return std::make_unique<ExprStmt>(std::move(root));    
}


// statement is an expression with ; at the end
//  expression;
static std::unique_ptr<Expr> parseStatement(Parser& parser) {
  if(getCurrentTokenType(parser) == TOKEN_PRINT) {
    consume("print", parser); 
    auto root = parseExpression(parser);
    if(!root) {
      std::cerr << "Root is null" << std::endl;
      return nullptr;
    }
    consume(";", parser);
    return std::make_unique<PrintStmt>(std::move(root)); 
    
  } else if(getCurrentTokenType(parser) == TOKEN_RETURN) {
     
    return parseReturnStatement(parser);
  } else {
    auto root = parseExpression(parser); 
    consume(";", parser);
    return root;
  }
}

static std::unique_ptr<ExprProto> parseProto(Parser& parser) {
  std::string id_name = getCurrentTokenView(parser);
  parser.advance();
  
  if(getCurrentTokenType(parser) != TOKEN_LEFT_PAREN) {
    std::cerr << "Expected '('" << std::endl;
    return nullptr;  
  }
  
  std::vector<std::string> arguments;
  if(getTokenType(parser.peek_next_token()) == TOKEN_RIGHT_PAREN) {
    parser.advance();
    parser.advance();
    return std::make_unique<ExprProto>(std::move(id_name), std::move(arguments)); 
  }
  
  while(getCurrentTokenType(parser) != TOKEN_RIGHT_PAREN) {
    if(getCurrentTokenType(parser) != TOKEN_IDENTIFIER) {
    parser.advance();
    continue;
    
    }
    auto arg = getCurrentTokenView(parser);  
    
    arguments.push_back(arg);
    parser.advance();        
    }

  parser.advance(); // eat ')'

  return std::make_unique<ExprProto>(std::move(id_name), std::move(arguments));
}

static std::unique_ptr<Expr> parseFunDeclaration(Parser& parser) {  
  parser.advance(); // eat 'function'
  
  // parse prototype - function tag and args
   auto proto = parseProto(parser); 
    if(!proto) {
      std::cerr << "Couldnt parse prototype" << std::endl;
      return nullptr;
    }
 
   // parse block statement - '{' + declaration + '}'
   consume("{", parser); 
    
    std::vector<std::unique_ptr<Expr>> stmts;
    while(getCurrentTokenType(parser) != TOKEN_RIGHT_BRACKET){
      
      auto root = parseDeclaration(parser);
      if(!root) {
        std::cerr << "Couldnt parse statement" << std::endl;
        return nullptr;
      }
      stmts.push_back(std::move(root));       
       
    }                

    consume("}", parser);  
   // compile function declaration
  return std::make_unique<FunDeclaration>(
      std::move(stmts), std::move(proto));
}

static std::unique_ptr<Expr> parseLetDeclaration(Parser& parser) {
 consume("let", parser); 
  
  std::string var_name = getCurrentTokenView(parser);
  consume(var_name, parser); 
   
  consume("=", parser); 
  auto root = parseExpression(parser);
  if(!root) {
    std::cerr <<  "Failed to parse expression";
    return nullptr;
  }
   consume(";", parser); 
  return std::make_unique<VarDeclaration>(std::move(var_name), std::move(root));
}


// declaration is func declaration | var declaration |   statment
static std::unique_ptr<Expr> parseDeclaration(Parser& parser) {
  switch(getCurrentTokenType(parser)){
    case TOKEN_FUN:
      return parseFunDeclaration(parser);
    break;
    case TOKEN_LET:
      return parseLetDeclaration(parser);
    break; 
    default: return parseStatement(parser);
  }  
}

void Parser::init(){ 
  FILE* out = fopen("main.ssa", "w+");
  if(!out) return;
  fprintf(out , "data $fmt_int = {b\"%%d\", b 0}\n"); 
  while(current < lexer->tokenize().size()) {   
    int stack_size = 0;
    auto root = parseDeclaration(*this);  
    if(!root) {
      std::cerr << "ROOT IS NULL";
      break;
    }
    root->generateCode(out, &stack_size);
    
  }  
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
