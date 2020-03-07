#ifndef ESCHER_COLOR_VIEW_H
#define ESCHER_COLOR_VIEW_H

#include <escher/transparent_view.h>

class ColorView : public TransparentView {
public:
  ColorView();
  KDColor color();
  int colorIndex();
  void setColor(int color);
  void setBackgroundColor(KDColor color);
  void setActive(bool active);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  int m_selectedColor;
  int m_numColors = 8;
  constexpr static KDCoordinate k_colorBoxSize = 18;
  KDColor m_backgroundColor;
  bool m_active;
};

#endif
