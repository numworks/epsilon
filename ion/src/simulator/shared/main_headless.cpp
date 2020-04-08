#include "main.h"
#include "display.h"
#include "platform.h"
#include "framebuffer.h"
#include "events.h"

#include <ion.h>
#include <ion/timing.h>
#include <ion/events.h>
#include <stdlib.h>
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif

constexpr int kHeapSize = 131072;
#ifdef NDEBUG
constexpr int kStackSize = 32768;
#else
constexpr int kStackSize = 32768*2; // In DEBUG mode, we increase the stack to be able to pass the tests
#endif

char heap[kHeapSize];
extern "C" {
  char * _heap_start = (char *)heap;
  char * _heap_end = _heap_start+kHeapSize;
}

void Ion::Timing::msleep(uint32_t ms) {
}

int main(int argc, char * argv[]) {
  Ion::Simulator::Framebuffer::setActive(false);
  // Parse command-line arguments
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "--logAfter") == 0 && argc > i+1) {
      Ion::Simulator::Framebuffer::setActive(true);
      Ion::Simulator::Events::logAfter(atoi(argv[i+1]));
    }
  }

#ifndef __WIN32__
  // Handle signals
  signal(SIGABRT, Ion::Simulator::Events::dumpEventCount);

  // Limit stack usage
  struct rlimit stackLimits = {kStackSize, kStackSize};
  setrlimit(RLIMIT_STACK, &stackLimits);
#endif

  ion_main(argc, argv);
  return 0;
}

namespace Ion {
namespace Simulator {
namespace Main {

void init() {
}

void setNeedsRefresh() {
}

void relayout() {
}

void refresh() {
}
}
}
}
