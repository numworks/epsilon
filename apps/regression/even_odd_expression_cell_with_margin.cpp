#include "even_odd_expression_cell_with_margin.h"

namespace Regression {

void EvenOddExpressionCellWithMargin::layoutSubviews() {
  KDRect boundsThis = bounds();
  m_expressionView.setFrame(KDRect(boundsThis.topLeft(), boundsThis.width() - k_rightMargin, boundsThis.height()));
}

}
