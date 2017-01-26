#ifndef POINCARE_PRODUCT_LAYOUT_H
#define POINCARE_PRODUCT_LAYOUT_H

#include "symbol_layout.h"

class ProductLayout : public SymbolLayout {
public:
  using SymbolLayout::SymbolLayout;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  constexpr static KDCoordinate k_lineThickness = 1;
};

#endif
