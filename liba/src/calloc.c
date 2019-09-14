#include <stdlib.h>

void * calloc(size_t count, size_t size) {
  void * result = malloc(count * size);
  if (result != NULL) {
    bzero(result, count * size);
  }
  return result;
}
