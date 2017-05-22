#include "pattern.h"

namespace HardwareTest {

constexpr KDColor Pattern::k_fillColors[Pattern::k_numberOfPatterns];
constexpr KDColor Pattern::k_outlineColors[Pattern::k_numberOfPatterns];

int Pattern::numberOfPatterns() {
  return k_numberOfPatterns;
}

Pattern Pattern::patternAtIndex(int i) {
  return Pattern(i);
}

KDColor Pattern::fillColor() const {
  return k_fillColors[m_i];
}

KDColor Pattern::outlineColor() const {
  return k_outlineColors[m_i];
}

Pattern::Pattern(int i) :
  m_i(i)
{
}

}
