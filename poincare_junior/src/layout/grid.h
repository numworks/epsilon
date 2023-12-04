#ifndef POINCARE_JUNIOR_LAYOUT_GRID_H
#define POINCARE_JUNIOR_LAYOUT_GRID_H

#include <poincare_junior/src/memory/tree.h>

#include "empty_rectangle.h"
#include "indices.h"
#include "k_tree.h"

namespace PoincareJ {

class Grid : public Tree {
 public:
  constexpr static KDCoordinate k_gridEntryMargin = 6;

  static const Grid* From(const Tree* node) {
    assert(node->isGridLayout());
    return static_cast<const Grid*>(node);
  }

  static Grid* From(Tree* node) {
    assert(node->isGridLayout());
    return static_cast<Grid*>(node);
  }

  uint8_t numberOfRows() const { return nodeValue(0); }
  uint8_t numberOfColumns() const { return nodeValue(1); }
  void setNumberOfRows(uint8_t rows) { setNodeValue(0, rows); }
  void setNumberOfColumns(uint8_t columns) { setNodeValue(1, columns); }

  const Tree* childAt(uint8_t col, uint8_t row) const;
  Tree* childAt(uint8_t col, uint8_t row) {
    return const_cast<Tree*>(const_cast<const Grid*>(this)->childAt(col, row));
  }

  Tree* willFillEmptyChildAtIndex(int childIndex);
  int removeTrailingEmptyRowOrColumnAtChildIndex(int childIndex);

  int rowAtChildIndex(int index) const;
  int columnAtChildIndex(int index) const;
  int indexAtRowColumn(int row, int column) const;
  int rightmostNonGrayColumn() const {
    return numberOfColumns() - 1 - (isEditing() && !numberOfColumnsIsFixed());
  }
  int closestNonGrayIndex(int index) const;

  void deleteColumnAtIndex(int index);
  void deleteRowAtIndex(int index);

  // Virtuality
  KDCoordinate horizontalGridEntryMargin(KDFont::Size font) const {
    return isPiecewiseLayout()
               ? 2 * k_gridEntryMargin + KDFont::GlyphWidth(font)
               : k_gridEntryMargin;
  }
  KDCoordinate verticalGridEntryMargin(KDFont::Size font) const {
    return k_gridEntryMargin;
  }
  bool numberOfRowsIsFixed() const { return false; }
  bool numberOfColumnsIsFixed() const { return isPiecewiseLayout(); }

  // Sizes
  KDCoordinate rowBaseline(int row, KDFont::Size font) const;
  KDCoordinate rowHeight(int row, KDFont::Size font) const;
  KDCoordinate height(KDFont::Size font) const;
  KDCoordinate columnWidth(int column, KDFont::Size font) const;
  KDCoordinate width(KDFont::Size font) const;
  KDSize size(KDFont::Size font) const {
    return KDSize(width(font), height(font));
  }
  KDPoint positionOfChildAt(int column, int row, KDFont::Size font) const;

  constexpr static int k_minimalNumberOfRowsAndColumnsWhileEditing = 2;

  // Row and columns
  bool isEditing() const;
  bool childIsPlaceholder(int index) const {
    return childIsRightOfGrid(index) || childIsBottomOfGrid(index);
  }

  bool isColumnEmpty(int index) const {
    return isColumnOrRowEmpty(true, index);
  }
  bool isRowEmpty(int index) const { return isColumnOrRowEmpty(false, index); }
  void addEmptyColumn();
  void addEmptyRow();
  bool childIsRightOfGrid(int index) const;
  bool childIsLeftOfGrid(int index) const;
  bool childIsTopOfGrid(int index) const;
  bool childIsBottomOfGrid(int index) const;
  bool childIsInLastNonGrayColumn(int index) const;
  bool childIsInLastNonGrayRow(int index) const;

 private:
  bool isColumnOrRowEmpty(bool column, int index) const;
  void addEmptyRowOrColumn(bool column);
  void colorGrayEmptyLayoutsInYellowInColumnOrRow(bool column, int lineIndex);
};

}  // namespace PoincareJ
#endif
