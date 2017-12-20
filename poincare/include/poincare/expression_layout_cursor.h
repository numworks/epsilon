#ifndef POINCARE_EXPRESSION_LAYOUT_CURSOR_H
#define POINCARE_EXPRESSION_LAYOUT_CURSOR_H

#include <poincare/expression_layout.h>
#include <kandinsky/point.h>

namespace Poincare {

class ExpressionLayoutCursor {
public:
  enum class Position {
    Left,
    Inside,
    Right
  };

  ExpressionLayoutCursor() :
    m_pointedExpressionLayout(nullptr),
    m_position(Position::Right),
    m_positionInside(0)
  {};

  /* Getters and setters */
  ExpressionLayout * pointedExpressionLayout() { return m_pointedExpressionLayout; }
  void setPointedExpressionLayout(ExpressionLayout * expressionLayout) { m_pointedExpressionLayout = expressionLayout; }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  int positionInside() const { return m_positionInside; }
  void setPositionInside(int positionInside) { m_positionInside = positionInside; }
  KDCoordinate cursorHeight() const { return k_cursorHeight; }

  /* Comparison */
  bool positionIsEquivalentTo(ExpressionLayout * expressionLayout, Position position, int positionIndex = 0);

  /* Position */
  KDPoint middleLeftPoint();
  KDPoint middleLeftPointOfCursor(ExpressionLayout * expressionLayout, Position position, int positionInside = 0);

  /* Move */
  bool moveLeft();
  bool moveRight();
  bool moveUp();
  bool moveDown();

  /* Edition */
  void addLayout(ExpressionLayout * layout);
  ExpressionLayout * addEmptyExponentialLayout();
  ExpressionLayout * addEmptyFractionLayout();
  ExpressionLayout * addEmptyLogarithmLayout();
  ExpressionLayout * addEmptyPowerLayout();
  ExpressionLayout * addEmptyRootLayout();
  ExpressionLayout * addEmptySquarePowerLayout();
  ExpressionLayout * addXNTCharLayout();
  ExpressionLayout * insertText(const char * text);

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  ExpressionLayout * m_pointedExpressionLayout;
  Position m_position;
  int m_positionInside;
};

}

#endif
