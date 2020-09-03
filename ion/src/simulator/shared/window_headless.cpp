#include "window.h"
#include "display.h"
#include "framebuffer.h"
#include "events.h"

#include <stdlib.h>
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif

#ifdef NDEBUG
/* TODO : Reduce stack memory cost in prime factorization to allow running
 * tests with the actual stack size */
constexpr int kStackSize = 32768*10;
#else
constexpr int kStackSize = 32768*10; // In DEBUG mode, we increase the stack to be able to pass the tests
#endif

namespace Ion {
namespace Simulator {
namespace Window {

void init() {
  Ion::Simulator::Framebuffer::setActive(false);
  // This feature is lost for now
  /* Parse command-line arguments
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "--logAfter") == 0 && argc > i+1) {
      Ion::Simulator::Framebuffer::setActive(true);
      Ion::Simulator::Events::logAfter(atoi(argv[i+1]));
    }
  }
  */
#ifndef __WIN32__
  // Handle signals
  signal(SIGABRT, Ion::Simulator::Events::dumpEventCount);

  // Limit stack usage
  struct rlimit stackLimits = {kStackSize, kStackSize};
  setrlimit(RLIMIT_STACK, &stackLimits);
#endif
}

void quit() {
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
