#ifndef POINCARE_EXPRESSION_LAYOUT_CURSOR_H
#define POINCARE_EXPRESSION_LAYOUT_CURSOR_H

#include <poincare/expression_layout.h>
#include <kandinsky/point.h>

namespace Poincare {

class ExpressionLayoutCursor {
public:
  enum class Position {
    Left,
    Right
  };

  ExpressionLayoutCursor() :
    m_pointedExpressionLayout(nullptr),
    m_position(Position::Right)
  {};

  /* Getters and setters */
  ExpressionLayout * pointedExpressionLayout() { return m_pointedExpressionLayout; }
  void setPointedExpressionLayout(ExpressionLayout * expressionLayout) { m_pointedExpressionLayout = expressionLayout; }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  KDCoordinate cursorHeight() const { return k_cursorHeight; }

  /* Comparison */
  bool positionIsEquivalentTo(ExpressionLayout * expressionLayout, Position position);

  /* Position */
  KDPoint middleLeftPoint();
  KDPoint middleLeftPointOfCursor(ExpressionLayout * expressionLayout, Position position);

  /* Move */
  bool moveLeft();
  bool moveRight();
  bool moveUp();
  bool moveDown();

  /* Edition */
  void addLayout(ExpressionLayout * layout);
  void addEmptyExponentialLayout();
  void addFractionLayoutAndCollapseBrothers();
  void addEmptyMatrixLayout(int numberOfRows = 1, int numberOfColumns = 1);
  void addEmptyPowerLayout();
  void addEmptySquareRootLayout();
  void addEmptySquarePowerLayout();
  void addXNTCharLayout();
  void insertText(const char * text);
  void performBackspace();

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  bool baseForNewPowerLayout();
  ExpressionLayout * m_pointedExpressionLayout;
  Position m_position;
};

}

#endif
