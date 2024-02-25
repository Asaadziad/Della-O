#include "lexer.h"

#ifdef DEBUG_FLAG
#include <iostream>
#endif

#include <string>
#include <fstream>

                    
/* TOKEN FUNCTIONALITY  */
typedef enum {
  TOKEN_INTEGER,
  TOKEN_IDENTIFIER,

  TOKEN_SEMICOLON,
  TOKEN_EQUAL,
} TokenType;

struct token_t {
  TokenType   type;
  std::string literal;
};

Token makeToken(std::string literal, TokenType type) {
   Token t = new struct token_t;
   t->type  = type;
   t->literal = literal;
   return t;
}

#ifdef DEBUG_FLAG
static std::string stringfyType(TokenType type) {
  switch(type) {
    case TOKEN_INTEGER:
        return "TOKEN_INTEGER";
    break;
    case TOKEN_IDENTIFIER:
        return "TOKEN_IDENTIFIER";
    break;
    case TOKEN_EQUAL: 
        return "TOKEN_EQUAL";
    break;
    case TOKEN_SEMICOLON:
        return "TOKEN_SEMICOLON";
    break;
    default: return "";
  }
}
#endif
/* END OF TOKEN FUNCTIONALITY  */


Lexer::Lexer(const std::string filename){
  std::ifstream file(filename);

  if(file.is_open()) {
    char ch;
    while(file) { 
      ch = file.get();
      buffer += ch;
    }
    buffer += '\0';
    #ifdef DEBUG_FLAG
    //std::cout << buffer;
    #endif
  }
 cursor = 0;
}

Lexer::~Lexer() {
  for(int i = 0; i < tokens.size(); i++) {
    delete tokens[i];
  }
}

char Lexer::peek() {
  return buffer[cursor];
}

void Lexer::advance() {
  cursor++;
}

static void skipWhiteSpaces(Lexer* lexer) {
 char cursor = lexer->peekNext();
 while(cursor == ' ' || cursor == '\t' || cursor == '\r' || cursor == '\n') {
  lexer->advance();
  cursor = lexer->peekNext();
 } 
}

static bool isNumChar(char c) {
  return c <= '9' && c >= '0';
}

static bool isAlphChar(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char Lexer::peekNext() {
  return buffer[cursor + 1];
}

std::string readInteger(Lexer* lexer) {
  std::string num = "";
  
  while(true) {
     char c = lexer->peek(); 
     num += c;  
  
     if (isNumChar(lexer->peekNext())) {
      lexer->advance();  
     } else {
      break;
     }

  }

  
  return num;
}

std::string readIdent(Lexer* lexer) {
  std::string ident = "";
  
  while(true) {
    char c = lexer->peek();
    ident += c;
    
    if(isAlphChar(lexer->peekNext())) {
     lexer->advance();
    } else {
      break;
    }
  } 

  
  return ident;
}

std::vector<Token> Lexer::tokenize() {
  skipWhiteSpaces(this);
  
  
  while(peek() != '\0') {
   char current = peek();
   switch(current) {
     case ';':
        tokens.push_back(makeToken(";", TOKEN_SEMICOLON));
        break;
     case '=':
         tokens.push_back(makeToken("=", TOKEN_EQUAL));
        break;
    default:break;
   }
    
   if(isNumChar(current)) {
      std::string integer = readInteger(this);
      tokens.push_back(makeToken(integer, TOKEN_INTEGER));    
   }

   if(isAlphChar(current)) {
      std::string ident = readIdent(this);
      tokens.push_back(makeToken(ident, TOKEN_IDENTIFIER)); 
   }

   skipWhiteSpaces(this); 
   advance(); 
  }


  return tokens;
}

#ifdef DEBUG_FLAG
void Lexer::print() {
  std::cout << "Tokens:" << std::endl;
  for(auto token : tokens) {
    std::cout << "{ Literal: " << token->literal << ", type: " << stringfyType(token->type) << " }" << std::endl;
  }
}
#endif
