#ifndef POINCARE_RIGHT_PARENTHESIS_LAYOUT_H
#define POINCARE_RIGHT_PARENTHESIS_LAYOUT_H

#include <poincare/src/layout/parenthesis_layout.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class RightParenthesisLayout : public ParenthesisLayout {
  friend class BinomialCoefficientLayout;
  friend class SequenceLayout;
public:
  using ParenthesisLayout::ParenthesisLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ')');
  }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
  bool isRightParenthesis() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
