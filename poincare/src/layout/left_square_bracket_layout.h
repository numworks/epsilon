#ifndef POINCARE_LEFT_SQUARE_BRACKET_LAYOUT_H
#define POINCARE_LEFT_SQUARE_BRACKET_LAYOUT_H

#include <poincare/src/layout/square_bracket_layout.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class LeftSquareBracketLayout final : public SquareBracketLayout {
public:
  using SquareBracketLayout::SquareBracketLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, '[');
  }
  bool isLeftBracket() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
