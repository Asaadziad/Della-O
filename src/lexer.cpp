#include "lexer.h"

#ifdef DEBUG_FLAG
#include <iostream>
#endif

#include <string>
#include <fstream>

/*typedef struct token_t* Token;

class Lexer {
  std::string buffer;
  Token* tokens;
  public:
  Lexer(const std::string filename);
};*/

Lexer::Lexer(const std::string filename){
  std::ifstream file(filename);

  if(file.is_open()) {
    file >> buffer;
    #ifdef DEBUG_FLAG
    std::cout << buffer;
    #endif
  } 
}
