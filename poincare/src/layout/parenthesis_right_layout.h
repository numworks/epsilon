#ifndef POINCARE_PARENTHESIS_RIGHT_LAYOUT_H
#define POINCARE_PARENTHESIS_RIGHT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>
#include <poincare/src/layout/parenthesis_left_right_layout.h>

namespace Poincare {

class ParenthesisRightLayout : public ParenthesisLeftRightLayout {
public:
  using ParenthesisLeftRightLayout::ParenthesisLeftRightLayout;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
