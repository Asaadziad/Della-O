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

void Parser::init(){  
   
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
