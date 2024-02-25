#include <iostream>
#include "lexer.h"

int main() {
  Lexer lexer("main.A");
  lexer.tokenize(); 
  #ifdef DEBUG_FLAG
    lexer.print();
  #endif
  return 0;
}
