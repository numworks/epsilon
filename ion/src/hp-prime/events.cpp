#include <ion.h>
#include <assert.h>

namespace Ion {
namespace Events {

constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;

bool canRepeatEvent(Event e) {
  return (e == Events::Left || e == Events::Up || e == Events::Down || e == Events::Right || e == Events::Backspace);
}

Event getEvent(int * timeout) {
  return Events::None;
}

}
}
