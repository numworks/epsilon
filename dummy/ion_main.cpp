#include <ion/display.h>
#include <ion/events.h>
#include <ion/timing.h>

using namespace Ion;

void ion_main(int argc, const char *const argv[]) {
  int t;
  do {
    t = 300;
  } while (Ion::Events::getEvent(&t) != Ion::Events::Termination);
}
