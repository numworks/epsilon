#include <ion/rtc.h>

static Ion::RTC::Mode s_mode = Ion::RTC::Mode::HSE;

void Ion::RTC::setMode(Ion::RTC::Mode mode) {
  s_mode = mode;
}

Ion::RTC::Mode Ion::RTC::mode() {
  return s_mode;
}

void Ion::RTC::setDateTime(Ion::RTC::DateTime dateTime) {
}

Ion::RTC::DateTime Ion::RTC::dateTime() {
  return Ion::RTC::DateTime { 0, 0, 0, 1, 1, 2000, 6 };
}
