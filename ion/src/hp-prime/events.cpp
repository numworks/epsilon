#include <ion.h>
#include <assert.h>

namespace Ion {
namespace Events {

constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;

bool canRepeatEvent(Event e) {
  return (e == Events::Left || e == Events::Up || e == Events::Down || e == Events::Right || e == Events::Backspace);
}

static bool sleepWithTimeout(int duration, int * timeout) {
  if (*timeout >= duration) {
    msleep(duration);
    *timeout -= duration;
    return false;
  } else {
    msleep(*timeout);
    *timeout = 0;
    return true;
  }
}

Event getEvent(int * timeout) {
  while (true) {
    if (sleepWithTimeout(10, timeout)) {
      return Events::None;
    }
  }
}

}
}
