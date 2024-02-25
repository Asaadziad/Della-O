#include <iostream>
#include "lexer.h"

int main() {
  Lexer lexer("main.A");
  lexer.tokenize(); 
  
  return 0;
}
