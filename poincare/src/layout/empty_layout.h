#ifndef POINCARE_empty_layout_H
#define POINCARE_empty_layout_H

#include <poincare/static_layout_hierarchy.h>
#include <assert.h>

namespace Poincare {

class EmptyLayout : public StaticLayoutHierarchy<0> {
public:
  enum class Color {
    Yellow,
    Grey
  };
  EmptyLayout(Color color = Color::Yellow, bool visible = true);
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
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

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
  constexpr static KDCoordinate k_width = 7;
  constexpr static KDCoordinate k_height = 12;
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayoutCursor cursorVerticalOf(VerticalDirection direction, ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) override;
  bool m_isVisible;
  Color m_color;
};

}

#endif
