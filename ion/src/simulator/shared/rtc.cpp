#include <ion/rtc.h>
#include <time.h>
#include <cstdio>

static Ion::RTC::Mode s_mode = Ion::RTC::Mode::HSE;

void Ion::RTC::setMode(Ion::RTC::Mode mode) {
  s_mode = mode;
}

Ion::RTC::Mode Ion::RTC::mode() {
  return s_mode;
}

void Ion::RTC::setDateTime(Ion::RTC::DateTime dateTime) {
  printf("Ion::RTC::setDateTime(%02d:%02d:%02d %02d/%02d/%04d)\n", dateTime.tm_hour, dateTime.tm_min, dateTime.tm_sec, dateTime.tm_mday, dateTime.tm_mon, dateTime.tm_year);
}

Ion::RTC::DateTime Ion::RTC::dateTime() {
  time_t localTime = time(nullptr);
  struct tm *localTm = localtime(&localTime);

  return DateTime {
    localTm->tm_sec,
    localTm->tm_min,
    localTm->tm_hour,
    localTm->tm_mday,
    localTm->tm_mon + 1,
    localTm->tm_year + 1900,
    localTm->tm_wday != 0 ? localTm->tm_wday - 1 : 6,
  };
}
