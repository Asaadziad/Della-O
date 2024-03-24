#include "lexer.h"

#ifdef DEBUG_FLAG
#include <iostream>
#endif

#include <string>
#include <fstream>

                    
/* TOKEN FUNCTIONALITY  */

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

std::string getTokenLiteral(Token token) {
  if(!token) return "TOKEN_ERROR";
  return token->literal;
}
 
TokenType getTokenType(Token token){
  if(!token) {
    return TOKEN_ERROR;
  }
  return token->type; }

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
  }
 cursor = 0;
}

Lexer::~Lexer() {
  for(int i = 0; i < tokens.size(); i++) {
    delete tokens[i];
  }
}

inline constexpr char Lexer::peek() {
  return buffer[cursor];
}

inline constexpr char Lexer::peekNext() {
  return buffer[cursor + 1];
}

void Lexer::advance() {
  cursor++;
}

std::vector<Token>& Lexer::tokenize() {
  return tokens;
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

std::string readString(Lexer* lexer) {
  lexer->advance();
  std::string ident = "";
  while(true) {
    char c = lexer->peek();
    ident += c;
    
    lexer->advance();
    if(lexer->peek() == '"') {
      
      break;
    } 
  }
   
  return ident;
}

void Lexer::init() {
  while(peek() != '\0') {
   char current = peek();
   
   switch(current) {
     case ';':
        tokens.push_back(makeToken(";", TOKEN_SEMICOLON));
        break;
     case ':':
        if(peekNext() == '=') {
         tokens.push_back(makeToken(":=", TOKEN_INITIALZE));
         break;
        }
        tokens.push_back(makeToken(":", TOKEN_COLON));
        break;
     case ',':
        tokens.push_back(makeToken(",", TOKEN_COMMA));
        break;
     case '=':
         if(peekNext() == '=') {
          tokens.push_back(makeToken("==", TOKEN_EQUAL_EQUAL));
          break;
         } 
         tokens.push_back(makeToken("=", TOKEN_EQUAL));
        break;
     case '(':
        tokens.push_back((makeToken("(", TOKEN_LEFT_PAREN)));
        break;
     case ')':
        tokens.push_back((makeToken(")", TOKEN_RIGHT_PAREN)));
        break;
     case '{':
        tokens.push_back((makeToken("{", TOKEN_LEFT_BRACKET)));
        break;
     case '}':
        tokens.push_back((makeToken("}", TOKEN_RIGHT_BRACKET)));
        break;
     case '+':
        tokens.push_back((makeToken("+", TOKEN_PLUS)));
        break;
     case '-':
         tokens.push_back((makeToken("-", TOKEN_MINUS)));
        break;
     case '/':
        tokens.push_back((makeToken("/", TOKEN_DIVIDE)));
        break;
     case '*':
        tokens.push_back((makeToken("*", TOKEN_MULT)));
        break;
     case '"':{
        std::string input = readString(this);
        tokens.push_back((makeToken(input, TOKEN_STRING)));
              }
        break;   
    default:break;
   }
    
   if(isNumChar(current)) {
      std::string integer = readInteger(this);
      tokens.push_back(makeToken(integer, TOKEN_INTEGER));    
   }

   if(isAlphChar(current)) {
      std::string ident = readIdent(this);
      if(ident.compare("func") == 0) {
        tokens.push_back(makeToken(ident, TOKEN_FUN));
      } else if(ident.compare("let") == 0) {
        tokens.push_back(makeToken(ident, TOKEN_LET));
      } else if(ident.compare("return") == 0) {
        tokens.push_back(makeToken(ident, TOKEN_RETURN));
      } else if(ident.compare("print") == 0) {
        tokens.push_back(makeToken(ident, TOKEN_PRINT));
      } else {
        tokens.push_back(makeToken(ident, TOKEN_IDENTIFIER)); 
      }
   }

   skipWhiteSpaces(this); 
   advance(); 
  }

}

#ifdef DEBUG_FLAG
void printToken(Token token) {
    std::cout << "{ literal: " << token->literal << ", type: " << stringfyType(token->type) << " }" << std::endl;
}

void Lexer::print() {
  std::cout << "Tokens:" << std::endl;
  for(auto token : tokens) {
    std::cout << "{ literal: " << token->literal << ", type: " << stringfyType(token->type) << " }" << std::endl;
  }
}
#endif
