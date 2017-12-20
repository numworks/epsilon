#ifndef POINCARE_EMPTY_VISIBLE_LAYOUT_H
#define POINCARE_EMPTY_VISIBLE_LAYOUT_H

#include "empty_layout.h"
#include <assert.h>

namespace Poincare {

class EmptyVisibleLayout : public EmptyLayout {
public:
  EmptyVisibleLayout();
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  virtual KDSize computeSize() override;
  void computeBaseline() override;
private:
  constexpr static KDCoordinate k_width = 7;
  constexpr static KDCoordinate k_height = 13;
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 2;
  constexpr static KDCoordinate k_lineThickness = 1;

  KDColor m_fillRectColor;
};

}

#endif
