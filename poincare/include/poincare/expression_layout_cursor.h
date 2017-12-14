#ifndef POINCARE_EXPRESSION_LAYOUT_CURSOR_H
#define POINCARE_EXPRESSION_LAYOUT_CURSOR_H

#include <poincare/expression_layout.h>

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
    m_position(Position::Left),
    m_positionInside(0)
  {};

  /* Getters and setters */
  ExpressionLayout * pointedExpressionLayout() { return m_pointedExpressionLayout; }
  void setPointedExpressionLayout(ExpressionLayout * expressionLayout) { m_pointedExpressionLayout = expressionLayout; }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  int positionInside() const { return m_positionInside; }
  void setPositionInside(int positionInside) { m_positionInside = positionInside; }

  /* Move */
  bool moveLeft();
  bool moveRight();
  bool moveUp();
  bool moveDown();

private:
  ExpressionLayout * m_pointedExpressionLayout;
  Position m_position;
  int m_positionInside;
};

}

#endif
