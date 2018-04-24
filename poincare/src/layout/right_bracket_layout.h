#ifndef POINCARE_BRACKET_RIGHT_LAYOUT_H
#define POINCARE_BRACKET_RIGHT_LAYOUT_H

#include <poincare/src/layout/bracket_layout.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class RightBracketLayout : public BracketLayout {
public:
  using BracketLayout::BracketLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ']');
  }
  bool isRightBracket() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
