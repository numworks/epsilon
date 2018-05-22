#ifndef SHARED_MODEL_EXPRESSION_CELL_H
#define SHARED_MODEL_EXPRESSION_CELL_H

#include <escher.h>

namespace Shared {

class ModelExpressionCell : public EvenOddExpressionCell {
public:
  ModelExpressionCell();
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void layoutSubviews() override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDCoordinate k_margin = 5;
};

}

#endif
