#ifndef POINCARE_CHAR_LAYOUT_H
#define POINCARE_CHAR_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

class CharLayout : public StaticLayoutHierarchy<0> {
public:
  CharLayout(char c, KDText::FontSize fontSize = KDText::FontSize::Large);
  ExpressionLayout * clone() const override;

  // CharLayout
  char character() const { return m_char; }
  KDText::FontSize fontSize() const { return m_fontSize; }

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, m_char);
  }

  // Other
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDPoint positionOfChild(ExpressionLayout * child) override {
    assert(false);
    return KDPointZero;
  }
  KDSize computeSize() override;
  void computeBaseline() override;
  char m_char;
  KDText::FontSize m_fontSize;
};

}

#endif
