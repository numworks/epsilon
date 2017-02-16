#include <ion/log.h>
#include <stdio.h>

void ion_log_string(const char * message) {
  printf("%s", message);
  fflush(stdout);
}
