#include <string>
#include <vector>

typedef struct token_t* Token;

class Lexer {
  std::string buffer;
  size_t cursor;
  std::vector<Token> tokens;
  public:
  Lexer(const std::string filename);
  ~Lexer(); 
  char peek();
  std::vector<Token> tokenize();
  void advance();
  char peekNext();
  #ifdef DEBUG_FLAG
  void print();
  #endif

};
