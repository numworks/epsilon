#include <assert.h>
#include <ion/exam_mode.h>

namespace Ion {
namespace ExamMode {

Configuration::Configuration(Ruleset rules, Int flags)
    : m_internals{.fields = {
                      .configurable = rules == Ruleset::PressToTest,
                      .data = rules == Ruleset::PressToTest
                                  ? flags
                                  : static_cast<Int>(rules),
                      .clearBit = 0,
                  }} {
  assert(rules < Ruleset::NumberOfRulesets &&
         (rules == Ruleset::PressToTest || flags == 0));
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
