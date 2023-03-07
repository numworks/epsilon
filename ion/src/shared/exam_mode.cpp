#include <assert.h>
#include <ion/authentication.h>
#include <ion/exam_mode.h>
#include <ion/reset.h>

#include "led.h"

namespace Ion {
namespace ExamMode {

Configuration get() {
  Configuration config(PersistingBytes::read());
  if (config.isUninitialized() ||
      (Authentication::clearanceLevel() !=
           Authentication::ClearanceLevel::NumWorks &&
       config.isActive())) {
    /* The persisting bytes do not contain a valid exam mode (most likely
     * because of a botched install), or an unauthentified firmware ended up in
     * exam mode. */
    config = Configuration(Ruleset::Off);
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
      /* The device will reset on official firmware, and pick up the
       * configuration left in the persisting bytes. */
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
    LED::setLock(false);
    LED::setColor(KDColorBlack);
  }
}

// Class Configuration

Configuration::Configuration(Ruleset rules, Int flags)
    : m_internals{.fields = {
                      .configurable = rules == Ruleset::PressToTest,
                      .data = rules == Ruleset::PressToTest
                                  ? flags
                                  : static_cast<Int>(rules),
                      .clearBit = 0,
                  }} {
  assert(!isUninitialized());
}

Ruleset Configuration::ruleset() const {
  assert(!isUninitialized());
  return m_internals.fields.configurable
             ? Ruleset::PressToTest
             : static_cast<Ruleset>(m_internals.fields.data);
}

Int Configuration::flags() const {
  assert(!isUninitialized());
  return m_internals.fields.configurable ? m_internals.fields.data : 0;
}

bool Configuration::isUninitialized() const {
  return m_internals.fields.clearBit ||
         (!m_internals.fields.configurable &&
          m_internals.fields.data >=
              static_cast<Int>(Ruleset::NumberOfRulesets));
}

bool Configuration::isActive() const {
  assert(!isUninitialized());
  return ruleset() != Ruleset::Off;
}

KDColor Configuration::color() const {
  assert(!isUninitialized());
  constexpr KDColor k_standardLEDColor = KDColorRed;
  constexpr KDColor k_portugueseLEDColor = KDColorGreen;
  /* The Dutch exam mode LED is supposed to be orange but, due to a disparity in
   * plastic quality, this color needs to be toned down closer to a yellow so
   * that it cannot be confused with the red color. In addition, light guide
   * tends to filter green light. 0xA1FF00 is a yellowish green, but appear as a
   * orangish yellow on most calculators, and is good enough for all plastic
   * disparities. Note : pure Orange LED is already used when the battery is
   * charging. */
  constexpr KDColor k_dutchLEDColor = KDColor::RGB24(0xA1FF00);

  switch (ruleset()) {
    case Ruleset::Standard:
      return k_standardLEDColor;
    case Ruleset::Dutch:
      return k_dutchLEDColor;
    case Ruleset::Portuguese:
      return k_portugueseLEDColor;
    default:
      return KDColorBlack;
  }
}

}  // namespace ExamMode
}  // namespace Ion
