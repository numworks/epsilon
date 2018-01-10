#include <ion/clock.h>

void Ion::Clock::setClock(int hours, int mins) {
}

void Ion::Clock::clock(int *hours, int *mins) {
  if (hours)
    *hours = 0;
  if (mins)
    *mins = 0;
}
