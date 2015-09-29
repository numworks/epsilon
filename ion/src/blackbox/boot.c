#define HEAP_SIZE 65536
char heap[HEAP_SIZE];
char * _liba_heap_start = &heap[0];
char * _liba_heap_end = &heap[HEAP_SIZE];

int main(int argc, char * argv[]) {
  ion_app();
/*  while (1) {
  }*/
  return 0;
}
