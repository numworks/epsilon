#include <assert.h>
#include <ion/authentication.h>
#include <ion/exam_mode.h>
#include <ion/reset.h>

#include "led.h"

namespace Ion {
namespace ExamMode {

Configuration get() {
  Configuration config(PersistingBytes::read());
  if (config.isUninitialized()) {
    /* The persisting bytes do not contain a valid exam mode, most likely
     * because of a botched install. */
    config = Configuration(Mode::Off);
    set(config);
  }
  return config;
}

void set(Configuration config) {
  assert(!config.isUninitialized());
  Configuration previous = get();
  PersistingBytes::write(config.raw());

  if (config.isActive()) {
    /* Exam mode should be deactivated before setting a new mode. */
    assert(!previous.isActive());

    if (Authentication::clearanceLevel() !=
        Authentication::ClearanceLevel::NumWorks) {
      Reset::core();
    }

    constexpr uint16_t blinkPeriod = 1000;  // in ms
    constexpr float blinkDutyCycle = 0.1f;
    KDColor color = config.color();
    if (color != KDColorBlack) {
      LED::setColor(config.color());
      LED::setBlinking(blinkPeriod, blinkDutyCycle);
      LED::setLock(true);
    }
  } else {
    if (previous.mode() == Mode::PressToTest) {
      Reset::core();
    }

    LED::setLock(false);
    LED::setColor(KDColorBlack);
  }
}

}  // namespace ExamMode
}  // namespace Ion
