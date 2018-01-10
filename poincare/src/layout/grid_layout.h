#ifndef POINCARE_GRID_LAYOUT_H
#define POINCARE_GRID_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>
#include "empty_visible_layout.h"

namespace Poincare {

class GridLayout : public DynamicLayoutHierarchy {
  friend class BinomialCoefficientLayout;
public:
  GridLayout(const ExpressionLayout * const * entryLayouts, int numberOfRows, int numberOfColumns, bool cloneOperands);
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Dynamic layout */
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;
  // TODO: This function replaces the child with an EmptyVisibleLayout. Is this
  // ok? If we want to delete the grid's children, we have to make sure no to
  // call this function.

  /* Expression engine */
  int writeTextInBuffer(char * buffer, int bufferSize) const override { return 0; }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void addEmptyRow(EmptyVisibleLayout::Color color);
  void addEmptyColumn(EmptyVisibleLayout::Color color);
  bool childIsRightOfGrid(int index) const;
  bool childIsBottomOfGrid(int index) const;
  int rowAtIndex(int index) const;
  int m_numberOfRows;
  int m_numberOfColumns;
private:
  constexpr static KDCoordinate k_gridEntryMargin = 6;
  KDCoordinate rowBaseline(int i);
  KDCoordinate rowHeight(int i);
  KDCoordinate height();
  KDCoordinate columnWidth(int j);
  KDCoordinate width();
  bool childIsLeftOfGrid(int index) const;
  bool childIsTopOfGrid(int index) const;
  int columnAtIndex(int index) const;

};

}

#endif
