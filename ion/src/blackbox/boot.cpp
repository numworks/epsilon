#include <ion.h>
#include <stdlib.h>
#include <signal.h>
#include "events.h"
#include "display.h"

constexpr int kHeapSize = 131072;
char heap[kHeapSize];
extern "C" {
  char * _heap_start = (char *)heap;
  char * _heap_end = _heap_start+kHeapSize;
  int main(int argc, char * argv[]);
}

int main(int argc, char * argv[]) {
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "--logAfter") == 0 && argc > i+1) {
      Ion::Display::Blackbox::setFrameBufferActive(true);
      Ion::Events::Blackbox::logAfter(atoi(argv[i+1]));
    }
  }
  signal(SIGABRT, Ion::Events::Blackbox::dumpEventCount);
  ion_main(argc, argv);
  return 0;
}
