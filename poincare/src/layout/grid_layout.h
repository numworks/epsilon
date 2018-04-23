#ifndef POINCARE_GRID_LAYOUT_H
#define POINCARE_GRID_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>
#include "empty_layout.h"

namespace Poincare {

class GridLayout : public DynamicLayoutHierarchy {
  friend class BinomialCoefficientLayout;
public:
  GridLayout(const ExpressionLayout * const * entryLayouts, int numberOfRows, int numberOfColumns, bool cloneOperands);
  ExpressionLayout * clone() const override;

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // Remove
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;
  /* This function replaces the child with an EmptyLayout. To delete the grid's
   * children, do not call this function. */

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize) const override { return 0; }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void addEmptyRow(EmptyLayout::Color color);
  void addEmptyColumn(EmptyLayout::Color color);
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
