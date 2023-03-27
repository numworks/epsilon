#ifndef ESCHER_GAUGE_VIEW_H
#define ESCHER_GAUGE_VIEW_H

#include <escher/cell_widget.h>
#include <escher/transparent_view.h>

namespace Escher {

class GaugeView : public TransparentView, public CellWidget {
 public:
  GaugeView();
  float level();
  void setLevel(float level);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_indicatorDiameter = 10;

  static int DirectionForEvent(Ion::Events::Event event) {
    return event == Ion::Events::Right || event == Ion::Events::Plus
               ? 1
               : (event == Ion::Events::Left || event == Ion::Events::Minus
                      ? -1
                      : 0);
  }

  // CellWidget
  const View* view() const override { return this; }
  void setHighlighted(bool highlighted) override;
  bool canBeActivatedByEvent(Ion::Events::Event event) const override {
    return DirectionForEvent(event) != 0;
  }

 private:
  constexpr static KDCoordinate k_thickness = 2;
  float m_level;
  KDColor m_backgroundColor;
};

}  // namespace Escher

#endif
