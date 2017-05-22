#ifndef HARDWARE_TEST_PATTERN_VIEW_H
#define HARDWARE_TEST_PATTERN_VIEW_H

#include <escher.h>
#include "pattern.h"

namespace HardwareTest {

class PatternView : public View {
public:
  PatternView();
  void setPattern(Pattern p);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_outlineThickness = 1;
  Pattern m_pattern;
};

}

#endif

