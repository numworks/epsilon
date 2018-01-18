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

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Dynamic layout */
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;
  // This function replaces the child with an EmptyVisibleLayout. To delete the
  // grid's children, do not call this function.

  /* Expression engine */
  int writeTextInBuffer(char * buffer, int bufferSize) const override { return 0; }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void addEmptyRow(EmptyVisibleLayout::Color color);
  void addEmptyColumn(EmptyVisibleLayout::Color color);
  void deleteRowAtIndex(int index);
  void deleteColumnAtIndex(int index);
  bool childIsRightOfGrid(int index) const;
  bool childIsLeftOfGrid(int index) const;
  bool childIsTopOfGrid(int index) const;
  bool childIsBottomOfGrid(int index) const;
  int rowAtChildIndex(int index) const;
  int columnAtChildIndex(int index) const;
  int indexAtRowColumn(int rowIndex, int columnIndex) const;
  int m_numberOfRows;
  int m_numberOfColumns;
private:
  constexpr static KDCoordinate k_gridEntryMargin = 6;
  KDCoordinate rowBaseline(int i);
  KDCoordinate rowHeight(int i);
  KDCoordinate height();
  KDCoordinate columnWidth(int j);
  KDCoordinate width();
};

}

#endif
