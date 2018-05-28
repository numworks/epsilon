#ifndef APPS_STATISTICS_CALCULATION_CELL_H
#define APPS_STATISTICS_CALCULATION_CELL_H

#include <escher/even_odd_buffer_text_cell.h>

namespace Statistics {

class CalculationCell : public EvenOddBufferTextCell {
public:
  using EvenOddBufferTextCell::EvenOddBufferTextCell;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
};

}

#endif
