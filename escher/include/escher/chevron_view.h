#ifndef ESCHER_CHEVRON_VIEW_H
#define ESCHER_CHEVRON_VIEW_H

#include <escher/arbitrary_shaped_view.h>
#include <escher/cell_widget.h>

namespace Escher {

class ChevronView : public ArbitraryShapedView, public CellWidget {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  /* k_chevronHeight and k_chevronWidth are the dimensions of the chevron. */
  constexpr static KDCoordinate k_chevronHeight = 10;
  constexpr static KDCoordinate k_chevronWidth = 8;

  // CellWidget
  const View* view() const override { return this; }
  bool canBeActivatedByEvent(Ion::Events::Event event) const override {
    return event == Ion::Events::Right ||
           CellWidget::canBeActivatedByEvent(event);
  }
};

}  // namespace Escher

#endif
