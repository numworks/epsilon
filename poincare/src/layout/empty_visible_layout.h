#ifndef POINCARE_EMPTY_VISIBLE_LAYOUT_H
#define POINCARE_EMPTY_VISIBLE_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <assert.h>

namespace Poincare {

class EmptyVisibleLayout : public StaticLayoutHierarchy<0> {
public:
  enum class Color {
    Yellow,
    Grey
  };
  EmptyVisibleLayout(Color color = Color::Yellow, bool visible = true);
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  bool isEmpty() const override { return true; }
  Color color() const { return m_color; }
  void setColor(Color color) { m_color = color; }
  bool isVisible() const { return m_isVisible; }
  void setVisible(bool visible) { m_isVisible = visible; }
protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  virtual KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override {
    assert(false);
    return KDPointZero;
  }
  void privateAddBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother, bool moveCursor) override;
private:
  constexpr static KDCoordinate k_width = 7;
  constexpr static KDCoordinate k_height = 13;
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  bool m_isVisible;
  Color m_color;
};

}

#endif
