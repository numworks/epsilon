#include <assert.h>
#include <ion/authentication.h>
#include <ion/exam_mode.h>
#include <ion/external_apps.h>
#include <ion/reset.h>

#include "led.h"

namespace Ion {
namespace ExamMode {

static void updateLed(Configuration config) {
  assert(!config.isUninitialized());
  if (config.isActive()) {
    constexpr uint16_t blinkPeriod = 1000;  // in ms
    constexpr float blinkDutyCycle = 0.1f;
    KDColor color = config.color();
    if (color != KDColorBlack) {
      LED::setColor(color);
      LED::setBlinking(blinkPeriod, blinkDutyCycle);
      LED::setLock(true);
    }
  } else {
    LED::setLock(false);
    LED::setColor(KDColorBlack);
  }
}

Configuration get() {
  if (Authentication::clearanceLevel() ==
      Authentication::ClearanceLevel::ThirdParty) {
    return Configuration(Ruleset::Off);
  }
  Configuration config(ExamBytes::read());
  if (config.isUninitialized() ||
      (Authentication::clearanceLevel() !=
           Authentication::ClearanceLevel::NumWorks &&
       config.isActive())) {
    /* The persisting bytes do not contain a valid exam mode (most likely
     * because of a botched install), or an unauthentified firmware ended up in
     * exam mode. */
    config = Configuration(Ruleset::Off);
    set(config);
    return config;
  }
  // Set LED the first time exam mode is retrieved
  updateLed(config);
  return config;
}

void set(Configuration config) {
  assert(!config.isUninitialized());
  if (Authentication::clearanceLevel() ==
      Authentication::ClearanceLevel::ThirdParty) {
    if (config.isActive()) {
      /* The device will reset on official firmware, and pick up the
       * configuration left in the other slot. */
      Reset::core();
    }
    return;
  }
#if ASSERTIONS
  Configuration previousConfig(ExamBytes::read());
#endif
  ExamBytes::write(config.raw());
  if (config.isActive() && Authentication::clearanceLevel() !=
                               Authentication::ClearanceLevel::NumWorks) {
    /* The device will reset on official firmware, and pick up the
     * configuration left in the persisting bytes. */
    Reset::core();
  }
  updateLed(config);
  if (!config.isActive()) {
#if ASSERTIONS
    assert(previousConfig.isActive());
    assert(Authentication::clearanceLevel() ==
           Authentication::ClearanceLevel::NumWorks);
#endif
    // Apps are visible again, it may require a clearance level update.
    Ion::ExternalApps::updateClearanceLevel(config.isActive());
  }
}

// Class Configuration

Configuration::Configuration(Ruleset rules, Int flags) : m_bits(0) {
  bool configurable = rules == Ruleset::PressToTest;
  // Store "configurable" boolean at Bits::Configurable
  m_bits = OMG::BitHelper::withBitsBetweenIndexes(
      m_bits, static_cast<size_t>(Bits::Configurable),
      static_cast<size_t>(Bits::Configurable), static_cast<Int>(configurable));
  // Store flags or ruleset between Bits::DataFirst and Bits::DataLast
  m_bits = OMG::BitHelper::withBitsBetweenIndexes(
      m_bits, static_cast<size_t>(Bits::DataLast),
      static_cast<size_t>(Bits::DataFirst),
      configurable ? flags : static_cast<Int>(rules));
}

Ruleset Configuration::ruleset() const {
  assert(!isUninitialized());
  return configurable() ? Ruleset::PressToTest : toRuleset(data());
}

Int Configuration::flags() const {
  assert(!isUninitialized());
  return configurable() ? data() : 0;
}

bool Configuration::isUninitialized() const {
  if (cleared()) {
    return true;
  }
  if (configurable()) {
    return false;
  }
  return toRuleset(data()) == Ruleset::Uninitialized;
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
