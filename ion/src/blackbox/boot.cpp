#include <ion.h>

constexpr int kHeapSize = 131072;
char heap[kHeapSize];
extern "C" {
  char * _heap_start = (char *)heap;
  char * _heap_end = _heap_start+kHeapSize;
  int main(int argc, char * argv[]);
}

int main(int argc, char * argv[]) {
  ion_app();
/*  while (1) {
  }*/
  return 0;
}
