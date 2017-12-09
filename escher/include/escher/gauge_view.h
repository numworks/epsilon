#ifndef ESCHER_GAUGE_VIEW_H
#define ESCHER_GAUGE_VIEW_H

#include <escher/transparent_view.h>

class GaugeView final : public TransparentView {
public:
  GaugeView() :
    m_level(1),
    m_backgroundColor(KDColorWhite) {}
  float level() { return m_level; }
  void setLevel(float level);
  void setBackgroundColor(KDColor color);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_indicatorDiameter = 10;
private:
  constexpr static KDCoordinate k_thickness = 2;
  float m_level;
  KDColor m_backgroundColor;
};

#endif
