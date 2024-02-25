#include <string>
#include <vector>

typedef struct token_t* Token;

class Lexer {
  std::string buffer;
  size_t cursor;
  std::vector<Token> tokens;
  public:
  Lexer(const std::string filename);
  char peek();
  std::vector<Token> tokenize();
  void advance();
};
