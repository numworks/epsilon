#ifndef POINCARE_empty_layout_H
#define POINCARE_empty_layout_H

#include <poincare/static_layout_hierarchy.h>
#include <kandinsky/text.h>
#include <assert.h>

namespace Poincare {

class EmptyLayout : public StaticLayoutHierarchy<0> {
public:
  enum class Color {
    Yellow,
    Grey
  };
  EmptyLayout(Color color = Color::Yellow, bool visible = true, KDText::FontSize size = KDText::FontSize::Large, bool margins = true);
  ExpressionLayout * clone() const override;

  // EmptyLayout
  Color color() const { return m_color; }
  void setColor(Color color) { m_color = color; }
  bool isVisible() const { return m_isVisible; }
  void setVisible(bool visible) { m_isVisible = visible; }

  // User input
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

  // Other
  bool isEmpty() const override { return true; }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override {
    assert(false);
    return KDPointZero;
  }
  void privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) override;
private:
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayoutCursor cursorVerticalOf(VerticalDirection direction, ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) override;
  KDCoordinate height() const { return KDText::charSize(m_size).height() - 2*k_marginHeight; }
  KDCoordinate width() const { return KDText::charSize(m_size).width() - 2*k_marginWidth; }
  bool m_isVisible;
  Color m_color;
  KDText::FontSize m_size;
  bool m_margins;
};

}

#endif
