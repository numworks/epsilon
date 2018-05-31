#ifndef APPS_REGRESSION_EVEN_ODD_EXPRESSION_CELL_WITH_MARGIN_H
#define APPS_REGRESSION_EVEN_ODD_EXPRESSION_CELL_WITH_MARGIN_H

#include <escher/even_odd_expression_cell.h>

namespace Regression {

class EvenOddExpressionCellWithMargin : public EvenOddExpressionCell {
public:
  using EvenOddExpressionCell::EvenOddExpressionCell;
  void layoutSubviews() override;
private:
  static constexpr KDCoordinate k_rightMargin = 2;
};

}

#endif
