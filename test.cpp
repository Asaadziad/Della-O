#include <unistd.h>
#include <cstring>

void print_string(const char* str) {
  size_t len = strlen(str);
  write(STDOUT_FILENO, str, sizeof(char) * len);
}

int main() {
  print_string("hello asaad\n"); 
  
  return 0;
}
