#ifndef POINCARE_PARENTHESIS_RIGHT_LAYOUT_H
#define POINCARE_PARENTHESIS_RIGHT_LAYOUT_H

#include <poincare/src/layout/parenthesis_left_right_layout.h>

namespace Poincare {

class ParenthesisRightLayout : public ParenthesisLeftRightLayout {
public:
  using ParenthesisLeftRightLayout::ParenthesisLeftRightLayout;
  ExpressionLayout * clone() const override;
  bool isRightParenthesis() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  void computeOperandHeight() override;
  void computeBaseline() override;
};

}

#endif
