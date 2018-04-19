#ifndef POINCARE_PARENTHESIS_LEFT_LAYOUT_H
#define POINCARE_PARENTHESIS_LEFT_LAYOUT_H

#include <poincare/src/layout/parenthesis_layout.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class ParenthesisLeftLayout : public ParenthesisLayout {
  friend class BinomialCoefficientLayout;
  friend class SequenceLayout;
public:
  using ParenthesisLayout::ParenthesisLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, '(');
  }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isLeftParenthesis() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
