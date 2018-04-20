#ifndef POINCARE_EXPRESSION_LAYOUT_CURSOR_H
#define POINCARE_EXPRESSION_LAYOUT_CURSOR_H

#include <kandinsky/point.h>

namespace Poincare {

class ExpressionLayout;

class ExpressionLayoutCursor {
public:
  enum class Position {
    Left,
    Right
  };

  ExpressionLayoutCursor(ExpressionLayout * layout = nullptr, Position position = Position::Right) :
    m_pointedExpressionLayout(layout),
    m_position(position)
  {};

  /* Definition */
  bool isDefined() const { return m_pointedExpressionLayout != nullptr; }

  /* Getters and setters */
  ExpressionLayout * pointedExpressionLayout() { return m_pointedExpressionLayout; }
  ExpressionLayout * pointedExpressionLayoutEquivalentChild();
  void setPointedExpressionLayout(ExpressionLayout * expressionLayout) { m_pointedExpressionLayout = expressionLayout; }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  KDCoordinate cursorHeight();
  KDCoordinate baseline();

  /* Comparison */
  bool isEquivalentTo(ExpressionLayoutCursor cursor);

  /* Position */
  KDPoint middleLeftPoint();

  /* Move */
  bool moveLeft(bool * shouldRecomputeLayout);
  bool moveRight(bool * shouldRecomputeLayout);
  bool moveUp(bool * shouldRecomputeLayout);
  bool moveDown(bool * shouldRecomputeLayout);

  /* Edition */
  void addLayoutAndMoveCursor(ExpressionLayout * layout);
  void addEmptyExponentialLayout();
  void addFractionLayoutAndCollapseSiblings();
  void addEmptyMatrixLayout(int numberOfRows = 1, int numberOfColumns = 1);
  void addEmptyPowerLayout();
  void addEmptySquareRootLayout();
  void addEmptySquarePowerLayout();
  void addXNTCharLayout();
  void insertText(const char * text);
  void performBackspace();
  bool showEmptyLayoutIfNeeded();
  bool hideEmptyLayoutIfNeeded();

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  bool baseForNewPowerLayout();
  bool privateShowHideEmptyLayoutIfNeeded(bool show);
  KDCoordinate pointedLayoutHeight();
  ExpressionLayout * equivalentPointedSiblingLayout();
  ExpressionLayout * m_pointedExpressionLayout;
  Position m_position;
};

}

#endif
