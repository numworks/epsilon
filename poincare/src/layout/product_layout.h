#ifndef POINCARE_PRODUCT_LAYOUT_H
#define POINCARE_PRODUCT_LAYOUT_H

#include "sequence_layout.h"

namespace Poincare {

class ProductLayout : public SequenceLayout {
public:
  using SequenceLayout::SequenceLayout;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

}

#endif
