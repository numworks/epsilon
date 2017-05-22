#ifndef HARDWARE_TEST_PATTERN_H
#define HARDWARE_TEST_PATTERN_H

#include <escher.h>

namespace HardwareTest {

class Pattern {
public:
  static int numberOfPatterns();
  static Pattern patternAtIndex(int i);
  KDColor fillColor() const;
  KDColor outlineColor() const;
private:
  Pattern(int i);
  int m_i;
  constexpr static int k_numberOfPatterns = 5;
  constexpr static KDColor k_fillColors[k_numberOfPatterns] = {KDColorBlack, KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  constexpr static KDColor k_outlineColors[k_numberOfPatterns] = {KDColorWhite, KDColorGreen, KDColorRed, KDColorRed, KDColorGreen};
};

}

#endif

