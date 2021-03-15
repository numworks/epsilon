#ifndef ION_RTC_H
#define ION_RTC_H

#include <stdint.h>

namespace Ion {
namespace RTC {

struct DateTime {
  int tm_sec;
  int tm_min;
  int tm_hour; // 0-23
  int tm_mday; // 1-31
  int tm_mon;  // 1-12
  int tm_year;
  int tm_wday; // 0-6, 0 is Monday
};

enum class Mode {
  Disabled = 0,
  LSI = 1,
  HSE = 2,
} ;

void setMode(Mode mode);
Mode mode();
void setDateTime(DateTime dateTime);
DateTime dateTime();

bool parseDate(const char * text, DateTime & target);
bool parseTime(const char * text, DateTime & target);
void toStringDate(DateTime dateTime, char *text);
void toStringTime(DateTime dateTime, char *text);

}
}

#endif
