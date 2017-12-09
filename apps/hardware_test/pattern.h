#ifndef HARDWARE_TEST_PATTERN_H
#define HARDWARE_TEST_PATTERN_H

#include <escher.h>

namespace HardwareTest {

class Pattern final {
public:
  static int numberOfPatterns() {
    return k_numberOfPatterns;
  }
  static Pattern patternAtIndex(int i) {
    return Pattern(i);
  }
  KDColor fillColor() const {
    return k_fillColors[m_i];
  }
  KDColor outlineColor() const {
    return k_outlineColors[m_i];
  }
private:
  Pattern(int i) : m_i(i) {}
  int m_i;
  constexpr static int k_numberOfPatterns = 5;
  constexpr static KDColor k_fillColors[k_numberOfPatterns] = {KDColorBlack, KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  constexpr static KDColor k_outlineColors[k_numberOfPatterns] = {KDColorWhite, KDColorGreen, KDColorRed, KDColorRed, KDColorGreen};
};

}

#endif

