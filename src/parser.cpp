#include "parser.h"
#include <iostream>
#include <memory>
#include "compiler.h"

Parser::Parser(std::string source) {
  Lexer     lexer(source);
  std::vector<Token> tokenss = lexer.tokenize();
  for(int i = 0; i < tokenss.size(); i++) {
    auto token =  makeToken(getTokenLiteral(tokenss[i]), getTokenType(tokenss[i]));  
    tokens.push_back(token);
  }
  current = 0; 
}

static std::string getCurrentTokenView(Parser& parser) {
  return getTokenLiteral(parser.peek_current());
}

/*
 * Function declaration
 * */

static std::unique_ptr<Expr> parseExpression(Parser& parser);
static std::unique_ptr<Expr> parseIdentifier(Parser& parser);
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
    
    return nullptr;
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
    case TOKEN_KEYWORD:
     parser.advance();
    return parseIdentifier(parser); 
    case TOKEN_PRINT:
    case TOKEN_IDENTIFIER:
    return parseIdentifier(parser);
    case TOKEN_INTEGER:
    return parseNumber(parser);
    case TOKEN_LEFT_PAREN: 
    return parseParen(parser);
    case TOKEN_LEFT_BRACKET:
    return parseBlock(parser); 
    default:
    std::cerr << getCurrentTokenView(parser); 
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
    case TOKEN_EQUAL: {
      parser.advance();
      auto RHS = parseExpression(parser);
      std::cout << "made assign";
      return std::make_unique<ExprBinary>(BINOP_ASSIGN, std::move(LHS), std::move(RHS));
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

static void parseStatement(Parser& parser) {
  if(getTokenType(parser.peek_current()) == TOKEN_PRINT) {
    
    auto root = parseExpression(parser);
    if(!root) {
      std::cerr << "Root is null" << std::endl;
      return;
    }
    compile(std::move(root)); 
    std::cout << "compiled" << std::endl;
  } else {
    
  }
}

static void parseDeclaration(Parser& parser) {
  parseStatement(parser);  
}

void Parser::init(){ 
  int stack_size = 0;
  while(current < tokens.size()) {   
    parseDeclaration(*this);  
    advance();
  }  
} 

Token Parser::peek_next_token() {
  if(current + 1 >= tokens.size()) return NULL;

  return tokens[current + 1];
}

void Parser::advance() {
  if(current >= tokens.size()) return;
  current++;
}
