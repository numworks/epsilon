#ifndef SHARED_FUNCTION_EXPRESSION_CELL_H
#define SHARED_FUNCTION_EXPRESSION_CELL_H

#include <escher.h>

namespace Shared {

class FunctionExpressionCell : public EvenOddExpressionCell {
public:
  FunctionExpressionCell() : EvenOddExpressionCell() {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
private:
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
};

}

#endif
