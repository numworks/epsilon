#ifndef REGRESSION_EVEN_ODD_BUFFER_TEXT_CELL_WITH_MARGIN_H
#define REGRESSION_EVEN_ODD_BUFFER_TEXT_CELL_WITH_MARGIN_H

#include <escher/even_odd_buffer_text_cell.h>

namespace Regression {

class EvenOddBufferTextCellWithMargin : public EvenOddBufferTextCell {
public:
  using EvenOddBufferTextCell::EvenOddBufferTextCell;
  void layoutSubviews() override;
private:
  static constexpr KDCoordinate k_horizontalMargin = 2;
};

}
#endif
