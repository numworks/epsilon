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
  EmptyVisibleLayout(Color color = Color::Yellow);
  ExpressionLayout * clone() const override;
  void addBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother) override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  bool isEmpty() const override { return true; }
  void setColor(Color color) { m_color = color; }
protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  virtual KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override {
    assert(false);
    return KDPointZero;
  }
private:
  constexpr static KDCoordinate k_width = 7;
  constexpr static KDCoordinate k_height = 13;
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  Color m_color;
};

}

#endif
