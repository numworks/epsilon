#pragma once

#include <escher/cell_widget.h>
#include <escher/toggleable_view.h>

namespace Escher {

class UnequalView : public View, public CellWidget {
  const View* view() const override { return this; }
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
};

class ToggleableUnequalView : public ToggleableView {
 public:
  using ToggleableView::ToggleableView;
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
};

}  // namespace Escher
