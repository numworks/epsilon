#ifndef ESCHER_UNEQUAL_VIEW_H
#define ESCHER_UNEQUAL_VIEW_H

#include <escher/cell_widget.h>
#include <escher/toggleable_view.h>

namespace Escher {

class UnequalView : public ToggleableView, public CellWidget {
 public:
  constexpr static KDCoordinate k_size = 9;
  const View* view() const override { return this; }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_state ? KDSize(k_size, k_size) : KDSizeZero;
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;
};

}  // namespace Escher

#endif
