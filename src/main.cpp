#include <iostream>
#include <memory>
#include <stdexcept>
#include "compiler.h"

int main() {
  Compiler c("main.della"); 
  c.compile(); 
  return 0;
}
