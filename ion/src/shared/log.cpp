#include <ion/log.h>
#include <stdio.h>

void Ion::Log::print(const char * message) {
  printf("%s\n", message);
}
