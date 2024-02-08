#include <string>

typedef struct token_t* Token;

class Lexer {
  std::string buffer;
  Token* tokens;
  public:
  Lexer(const std::string filename);
};
