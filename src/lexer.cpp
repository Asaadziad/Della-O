#include "lexer.h"

#ifdef DEBUG_FLAG
#include <iostream>
#endif

#include <string>
#include <fstream>

#define isNumber    '0':\
                    case '1':\
                    case '2':\
                    case '3':\
                    case '4':\
                    case '5':\
                    case '6':\
                    case '7':\
                    case '8':\
                    case '9'



/* TOKEN FUNCTIONALITY  */
typedef enum {
  TOKEN_INTEGER,
} TokenType;

struct token_t {
  TokenType   type;
  std::string literal;
};

Token makeToken(std::string literal, TokenType type) {
   Token* t = new Token;
   (*t)->type  = type;
   (*t)->literal = literal;
   return *t;
}


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
    std::cout << buffer;
    #endif
  }
 cursor = 0;
}

char Lexer::peek() {
  return buffer[cursor];
}

void Lexer::advance() {
  cursor++;
}

static void skipWhiteSpaces(Lexer* lexer) {
 if(1) {
  lexer->advance();
 } 
}

static bool isNumChar(char c) {
  return c < '9' && c > '0';
}

std::string readInteger(Lexer* lexer) {
  std::string num = "69";
  
  while(true) {
     char c = lexer->peek();
     lexer->advance();
     if(isNumChar(c)) {
      num += c;
      
     } else {
      break;
     }
  }

  return num;
}



std::vector<Token> Lexer::tokenize() {
  skipWhiteSpaces(this);
  
  
  while(peek() != '\0') {
   switch(peek()) {
    case isNumber:
         std::string integer = readInteger(this);
         tokens.push_back(makeToken(integer, TOKEN_INTEGER));
    #ifdef DEBUG_FLAG
         std::cout << integer;
    #endif

    break;
   }
  advance(); 
  }

  advance();

  return tokens;
}
