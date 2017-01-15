#ifndef POINCARE_SUM_LAYOUT_H
#define POINCARE_SUM_LAYOUT_H

#include "symbol_layout.h"

class SumLayout : public SymbolLayout {
public:
  using SymbolLayout::SymbolLayout;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

#endif
