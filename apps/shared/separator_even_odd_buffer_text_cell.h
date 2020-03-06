#ifndef APPS_SHARED_SEPARATOR_EVEN_ODD_CELL_H
#define APPS_SHARED_SEPARATOR_EVEN_ODD_CELL_H

#include <escher/even_odd_buffer_text_cell.h>
#include <escher/metric.h>

namespace Shared {

class SeparatorEvenOddBufferTextCell : public EvenOddBufferTextCell {
public:
  using EvenOddBufferTextCell::EvenOddBufferTextCell;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
private:
  constexpr static KDCoordinate k_rightMargin = Metric::CellMargin;
};

}

#endif
