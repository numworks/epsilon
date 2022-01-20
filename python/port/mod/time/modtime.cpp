extern "C" {
#include "modtime.h"
#include <py/runtime.h>
#include <py/smallint.h>
}
#include <ion/rtc.h>
#include <ion/timing.h>
#include "../../helpers.h"

mp_obj_t modtime_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
  micropython_port_interruptible_msleep(1000 * mp_obj_get_float(seconds_o));
#else
  micropython_port_interruptible_msleep(1000 * mp_obj_get_int(seconds_o));
#endif
  return mp_const_none;
}

mp_obj_t modtime_monotonic() {
    return mp_obj_new_float(Ion::Timing::millis() / 1000.0);
}

//
// Upsilon extensions, based off MicroPython's modutime.c
//

// LEAPOCH corresponds to 2000-03-01, which is a mod-400 year, immediately
// after Feb 29. We calculate seconds as a signed integer relative to that.
//
// Our timebase is relative to 2000-01-01.

constexpr int LEAPOCH = ((31 + 29) * 86400);

constexpr int DAYS_PER_400Y = (365 * 400 + 97);
constexpr int DAYS_PER_100Y = (365 * 100 + 24);
constexpr int DAYS_PER_4Y =   (365 * 4 + 1);

static const uint16_t days_since_jan1[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

bool timeutils_is_leap_year(mp_uint_t year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// month is one based
mp_uint_t timeutils_days_in_month(mp_uint_t year, mp_uint_t month) {
  mp_uint_t mdays = days_since_jan1[month] - days_since_jan1[month - 1];
  if (month == 2 && timeutils_is_leap_year(year)) {
    mdays++;
  }
  return mdays;
}

// compute the day of the year, between 1 and 366
// month should be between 1 and 12, date should start at 1
mp_uint_t timeutils_year_day(mp_uint_t year, mp_uint_t month, mp_uint_t date) {
  mp_uint_t yday = days_since_jan1[month - 1] + date;
  if (month >= 3 && timeutils_is_leap_year(year)) {
    yday += 1;
  }
  return yday;
}

Ion::RTC::DateTime timeutils_seconds_since_2000_to_struct_time(mp_uint_t t) {
  Ion::RTC::DateTime tm;

  // The following algorithm was adapted from musl's __secs_to_tm and adapted
  // for differences in MicroPython's timebase.

  mp_int_t seconds = t - LEAPOCH;

  mp_int_t days = seconds / 86400;
  seconds %= 86400;
  if (seconds < 0) {
    seconds += 86400;
    days -= 1;
  }
  tm.tm_hour = seconds / 3600;
  tm.tm_min = seconds / 60 % 60;
  tm.tm_sec = seconds % 60;

  mp_int_t wday = (days + 2) % 7;   // Mar 1, 2000 was a Wednesday (2)
  if (wday < 0) {
    wday += 7;
  }
  tm.tm_wday = wday;

  mp_int_t qc_cycles = days / DAYS_PER_400Y;
  days %= DAYS_PER_400Y;
  if (days < 0) {
    days += DAYS_PER_400Y;
    qc_cycles--;
  }
  mp_int_t c_cycles = days / DAYS_PER_100Y;
  if (c_cycles == 4) {
    c_cycles--;
  }
  days -= (c_cycles * DAYS_PER_100Y);

  mp_int_t q_cycles = days / DAYS_PER_4Y;
  if (q_cycles == 25) {
    q_cycles--;
  }
  days -= q_cycles * DAYS_PER_4Y;

  mp_int_t years = days / 365;
  if (years == 4) {
    years--;
  }
  days -= (years * 365);

  tm.tm_year = 2000 + years + 4 * q_cycles + 100 * c_cycles + 400 * qc_cycles;

  // Note: days_in_month[0] corresponds to March
  static const int8_t days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

  mp_int_t month;
  for (month = 0; days_in_month[month] <= days; month++) {
    days -= days_in_month[month];
  }

  tm.tm_mon = month + 2;
  if (tm.tm_mon >= 12) {
    tm.tm_mon -= 12;
    tm.tm_year++;
  }
  tm.tm_mday = days + 1; // Make one based
  tm.tm_mon++;   // Make one based
  return tm;
}

// returns the number of seconds, as an integer, since 2000-01-01
mp_uint_t timeutils_seconds_since_2000(Ion::RTC::DateTime tm) {
  return
    tm.tm_sec
    + tm.tm_min * 60
    + tm.tm_hour * 3600
    + (timeutils_year_day(tm.tm_year, tm.tm_mon, tm.tm_mday) - 1
      + ((tm.tm_year - 2000 + 3) / 4) // add a day each 4 years starting with 2001
      - ((tm.tm_year - 2000 + 99) / 100) // subtract a day each 100 years starting with 2001
      + ((tm.tm_year - 2000 + 399) / 400) // add a day each 400 years starting with 2001
      ) * 86400
    + (tm.tm_year - 2000) * 31536000;
}

mp_uint_t timeutils_mktime(Ion::RTC::DateTime tm) {
  // Normalize the tuple. This allows things like:
  //
  // tm_tomorrow = list(time.localtime())
  // tm_tomorrow[2] += 1 # Adds 1 to mday
  // tomorrow = time.mktime(tm_tomorrow)
  //
  // And not have to worry about all the weird overflows.
  //
  // You can subtract dates/times this way as well.

  tm.tm_min += tm.tm_sec / 60;
  if ((tm.tm_sec = tm.tm_sec % 60) < 0) {
    tm.tm_sec += 60;
    tm.tm_min--;
  }

  tm.tm_hour += tm.tm_min / 60;
  if ((tm.tm_min = tm.tm_min % 60) < 0) {
    tm.tm_min += 60;
    tm.tm_hour--;
  }

  tm.tm_mday += tm.tm_hour / 24;
  if ((tm.tm_hour = tm.tm_hour % 24) < 0) {
    tm.tm_hour += 24;
    tm.tm_mday--;
  }

  tm.tm_mon--; // make month zero based
  tm.tm_year += tm.tm_mon / 12;
  if ((tm.tm_mon = tm.tm_mon % 12) < 0) {
      tm.tm_mon += 12;
      tm.tm_year--;
  }
  tm.tm_mon++; // back to one based

  while (tm.tm_mday < 1) {
    if (--tm.tm_mon == 0) {
      tm.tm_mon = 12;
      tm.tm_year--;
    }
    tm.tm_mday += timeutils_days_in_month(tm.tm_year, tm.tm_mon);
  }
  while ((mp_uint_t)tm.tm_mday > timeutils_days_in_month(tm.tm_year, tm.tm_mon)) {
      tm.tm_mday -= timeutils_days_in_month(tm.tm_year, tm.tm_mon);
      if (++tm.tm_mon == 13) {
          tm.tm_mon = 1;
          tm.tm_year++;
      }
  }
  return timeutils_seconds_since_2000(tm);
}

mp_obj_t modtime_localtime(size_t n_args, const mp_obj_t *args) {
  Ion::RTC::DateTime tm;
  if (n_args == 0 || args[0] == mp_const_none) {
    tm = Ion::RTC::dateTime();
  } else {
    mp_int_t seconds = mp_obj_get_int(args[0]);
    tm = timeutils_seconds_since_2000_to_struct_time(seconds);
  }
  mp_obj_t tuple[8] = {
    tuple[0] = mp_obj_new_int(tm.tm_year),
    tuple[1] = mp_obj_new_int(tm.tm_mon),
    tuple[2] = mp_obj_new_int(tm.tm_mday),
    tuple[3] = mp_obj_new_int(tm.tm_hour),
    tuple[4] = mp_obj_new_int(tm.tm_min),
    tuple[5] = mp_obj_new_int(tm.tm_sec),
    tuple[6] = mp_obj_new_int(tm.tm_wday),
    tuple[7] = mp_obj_new_int(timeutils_year_day(tm.tm_year, tm.tm_mon, tm.tm_mday)),
  };
  return mp_obj_new_tuple(8, tuple);
}

mp_obj_t modtime_mktime(mp_obj_t tuple) {
  size_t len;
  mp_obj_t *elem;

  mp_obj_get_array(tuple, &len, &elem);

  // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
  if (len < 8 || len > 9) {
    mp_raise_TypeError("mktime needs a tuple of length 8 or 9");
  }

  Ion::RTC::DateTime tm {
    (int)mp_obj_get_int(elem[5]),
    (int)mp_obj_get_int(elem[4]),
    (int)mp_obj_get_int(elem[3]),
    (int)mp_obj_get_int(elem[2]),
    (int)mp_obj_get_int(elem[1]),
    (int)mp_obj_get_int(elem[0]),
  };

  return mp_obj_new_int_from_uint(timeutils_mktime(tm));
}

mp_obj_t modtime_time(void) {
  return mp_obj_new_int(timeutils_seconds_since_2000(Ion::RTC::dateTime()));
}

// Omega private extensions.

mp_obj_t modtime_rtcmode(void) {
  return mp_obj_new_int((int)Ion::RTC::mode());
}

mp_obj_t modtime_setrtcmode(mp_obj_t mode) {
  mp_int_t intMode = mp_obj_get_int(mode);
  if (intMode < (int)Ion::RTC::Mode::Disabled || intMode > (int)Ion::RTC::Mode::HSE) {
    mp_raise_ValueError("mode must be between 0 and 2");
  }
  Ion::RTC::setMode((Ion::RTC::Mode)intMode);
  return mp_const_none;
}

mp_obj_t modtime_setlocaltime(mp_obj_t tuple) {
  size_t len;
  mp_obj_t *elem;

  mp_obj_get_array(tuple, &len, &elem);

  if (len < 5) {
    mp_raise_TypeError("setlocaltime needs a tuple of length >= 5");
  }

  Ion::RTC::DateTime tm {
    len > 5 ? (int)mp_obj_get_int(elem[5]) : 0,
    (int)mp_obj_get_int(elem[4]),
    (int)mp_obj_get_int(elem[3]),
    (int)mp_obj_get_int(elem[2]),
    (int)mp_obj_get_int(elem[1]),
    (int)mp_obj_get_int(elem[0]),
  };

  Ion::RTC::setDateTime(tm);
  return mp_const_none;
}
