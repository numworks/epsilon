#ifndef POINCARE_RIGHT_SQUARE_BRACKET_LAYOUT_H
#define POINCARE_RIGHT_SQUARE_BRACKET_LAYOUT_H

#include <poincare/src/layout/square_bracket_layout.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class RightSquareBracketLayout : public SquareBracketLayout {
public:
  using SquareBracketLayout::SquareBracketLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ']');
  }
  bool isRightBracket() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

}

#endif
