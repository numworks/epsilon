#ifndef ESCHER_GAUGE_VIEW_H
#define ESCHER_GAUGE_VIEW_H

#include <escher/transparent_view.h>

namespace Escher {

class GaugeView : public TransparentView {
 public:
  GaugeView();
  float level();
  void setLevel(float level);
  void setBackgroundColor(KDColor color);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_indicatorDiameter = 10;

 private:
  constexpr static KDCoordinate k_thickness = 2;
  float m_level;
  KDColor m_backgroundColor;
};

}  // namespace Escher

#endif
