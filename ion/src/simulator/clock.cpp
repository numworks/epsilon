#include <ion/clock.h>
#include <time.h>

void Ion::Clock::setClock(int hours, int mins) {
}

void Ion::Clock::clock(int *hours, int *mins) {
  time_t localTime = time(nullptr);
  struct tm *localTm = localtime(&localTime);

  if (hours)
    *hours = localTm->tm_hour;
  if (mins)
    *mins = localTm->tm_min;
}
