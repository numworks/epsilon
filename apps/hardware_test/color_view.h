#ifndef HARDWARE_TEST_COLOR_VIEW_H
#define HARDWARE_TEST_COLOR_VIEW_H

#include <escher.h>

namespace HardwareTest {

class ColorView : public View {
public:
  ColorView();
  void setColors(KDColor fillColor, KDColor outlineColor);
  KDColor fillColor();
  KDColor outlineColor();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_outlineThickness = 1;
  KDColor m_fillColor;
  KDColor m_outlineColor;
};

}

#endif

