#ifndef SHARED_FUNCTION_EXPRESSION_CELL_H
#define SHARED_FUNCTION_EXPRESSION_CELL_H

#include <escher/even_odd_expression_cell.h>
#include <escher/message_table_cell_with_switch.h>

namespace Shared {

class FunctionExpressionCell : public Escher::EvenOddExpressionCell {
public:
  FunctionExpressionCell() : EvenOddExpressionCell() {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
};

}

#endif
