#include <assert.h>
#include <ion/authentication.h>
#include <ion/exam_mode.h>
#include <ion/led.h>
#include <ion/reset.h>

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

// class Configuration

Configuration::Configuration(Mode mode, Int flags)
    : m_internals{
          .configurable = mode == Mode::PressToTest,
          .data = mode == Mode::PressToTest ? flags : static_cast<Int>(mode),
          .clearBit = 0,
      } {
  assert(mode < Mode::NumberOfModes &&
         (mode == Mode::PressToTest || flags == 0));
}

Mode Configuration::mode() const {
  assert(!isUninitialized());
  return m_internals.configurable ? Mode::PressToTest
                                  : static_cast<Mode>(m_internals.data);
}

Int Configuration::flags() const {
  assert(!isUninitialized());
  return m_internals.configurable ? m_internals.data : 0;
}

bool Configuration::isUninitialized() const {
  return m_internals.clearBit ||
         (!m_internals.configurable &&
          m_internals.data >= static_cast<Int>(Mode::NumberOfModes));
}

bool Configuration::isActive() const {
  assert(!isUninitialized());
  return mode() != Mode::Off;
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

  switch (mode()) {
    case Mode::Standard:
      return k_standardLEDColor;
    case Mode::Dutch:
      return k_dutchLEDColor;
    case Mode::Portuguese:
      return k_portugueseLEDColor;
    default:
      return KDColorBlack;
  }
}

}  // namespace ExamMode
}  // namespace Ion
