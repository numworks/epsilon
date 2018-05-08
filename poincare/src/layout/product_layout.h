#ifndef POINCARE_PRODUCT_LAYOUT_H
#define POINCARE_PRODUCT_LAYOUT_H

#include "sequence_layout.h"
#include <poincare/layout_engine.h>

namespace Poincare {

class ProductLayout : public SequenceLayout {
public:
  using SequenceLayout::SequenceLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

}

#endif
