#include "grid.h"

#include "k_tree.h"
#include "rack_layout.h"

namespace PoincareJ {

void Grid::willFillEmptyChildAtIndex(int childIndex) {
  assert(RackLayout::IsEmpty(child(childIndex)));
  assert(isEditing());
  bool isBottomOfGrid = childIsBottomOfGrid(childIndex);
  bool isRightOfGrid = childIsRightOfGrid(childIndex);
  if (isRightOfGrid && !numberOfColumnsIsFixed()) {
    // assert(static_cast<HorizontalLayoutNode *>(childAtIndex(childIndex))
    // ->emptyColor() == EmptyRectangle::Color::Gray);
    colorGrayEmptyLayoutsInYellowInColumnOrRow(true, numberOfColumns() - 1);
    addEmptyColumn(EmptyRectangle::Color::Gray);
  }
  if (isBottomOfGrid && !numberOfRowsIsFixed()) {
    // assert(static_cast<HorizontalLayoutNode *>(childAtIndex(childIndex))
    // ->emptyColor() == EmptyRectangle::Color::Gray ||
    // isRightOfGrid);  // The empty color already changed if isRightOfGrid
    colorGrayEmptyLayoutsInYellowInColumnOrRow(false, numberOfRows() - 1);
    addEmptyRow(EmptyRectangle::Color::Gray);
  }
}

int Grid::removeTrailingEmptyRowOrColumnAtChildIndex(int childIndex) {
  assert(RackLayout::IsEmpty(child(childIndex)));
  assert(isEditing());
  int row = rowAtChildIndex(childIndex);
  int column = columnAtChildIndex(childIndex);
  bool isRightOfGrid = childIsInLastNonGrayColumn(childIndex);
  bool isBottomOfGrid = childIsInLastNonGrayRow(childIndex);
  int newColumn = column;
  int newRow = row;
  while (isRightOfGrid && !numberOfColumnsIsFixed() &&
         numberOfColumns() > k_minimalNumberOfRowsAndColumnsWhileEditing &&
         isColumnEmpty(column)) {
    newColumn = column;
    deleteColumnAtIndex(column--);
  }
  while (isBottomOfGrid && !numberOfRowsIsFixed() &&
         numberOfRows() > k_minimalNumberOfRowsAndColumnsWhileEditing &&
         isRowEmpty(row)) {
    newRow = row;
    deleteRowAtIndex(row--);
  }
  assert(numberOfColumns() >= k_minimalNumberOfRowsAndColumnsWhileEditing &&
         numberOfRows() >= k_minimalNumberOfRowsAndColumnsWhileEditing);
  return indexAtRowColumn(newRow, newColumn);
}

// Protected
void Grid::deleteRowAtIndex(int index) {
  assert(!numberOfRowsIsFixed());
  assert(index >= 0 && index < numberOfRows());
  /* removeChildAtIndexInPlace messes with the number of rows to keep it
   * consistent with the number of children */
  int nbColumns = numberOfColumns();
  int nbRows = numberOfRows();
  for (int i = 0; i < nbColumns; i++) {
    child(index * nbColumns)->removeTree();
  }
  setNumberOfRows(nbRows - 1);
}

void Grid::deleteColumnAtIndex(int index) {
  assert(!numberOfColumnsIsFixed());
  assert(index >= 0 && index < numberOfColumns());
  /* removeChildAtIndexInPlace messes with the number of rows to keep it
   * consistent with the number of children */
  int nbColumns = numberOfColumns();
  int nbRows = numberOfRows();
  for (int i = (nbRows - 1) * nbColumns + index; i > -1; i -= nbColumns) {
    child(i)->removeTree();
  }
  setNumberOfColumns(nbColumns - 1);
}

bool Grid::childIsLeftOfGrid(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return columnAtChildIndex(index) == 0;
}

bool Grid::childIsRightOfGrid(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return columnAtChildIndex(index) == numberOfColumns() - 1;
}

bool Grid::childIsTopOfGrid(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return rowAtChildIndex(index) == 0;
}

bool Grid::childIsBottomOfGrid(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return rowAtChildIndex(index) == numberOfRows() - 1;
}

bool Grid::childIsInLastNonGrayColumn(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return columnAtChildIndex(index) == numberOfColumns() - 1 - isEditing();
}

bool Grid::childIsInLastNonGrayRow(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return rowAtChildIndex(index) == numberOfRows() - 1 - isEditing();
}

int Grid::rowAtChildIndex(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return (int)(index / numberOfColumns());
}

int Grid::columnAtChildIndex(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return index - numberOfColumns() * rowAtChildIndex(index);
}

int Grid::indexAtRowColumn(int row, int column) const {
  assert(row >= 0 && row < numberOfRows());
  assert(column >= 0 && column < numberOfColumns());
  return row * numberOfColumns() + column;
}

int Grid::closestNonGrayIndex(int index) const {
  if (!isEditing()) {
    return index;
  }
  int row = rowAtChildIndex(index);
  int column = columnAtChildIndex(index);
  if (!numberOfColumnsIsFixed() && childIsRightOfGrid(index)) {
    column--;
  }
  if (!numberOfRowsIsFixed() && childIsBottomOfGrid(index)) {
    row--;
  }
  return indexAtRowColumn(row, column);
}

KDCoordinate Grid::rowBaseline(int row, KDFont::Size font) const {
  assert(numberOfColumns() > 0);
  KDCoordinate rowBaseline = 0;
  int column = 0;
  const Tree* l = child(row * numberOfColumns());
  while (column < numberOfColumns()) {
    rowBaseline = std::max(rowBaseline, Render::Baseline(l));
    column++;
    l = l->nextTree();
  }
  return rowBaseline;
}

KDCoordinate Grid::rowHeight(int row, KDFont::Size font) const {
  KDCoordinate underBaseline = 0;
  KDCoordinate aboveBaseline = 0;
  int column = 0;
  const Tree* l = child(row * numberOfColumns());
  while (column < numberOfColumns()) {
    KDCoordinate b = Render::Baseline(l);
    underBaseline =
        std::max<KDCoordinate>(underBaseline, Render::Height(l) - b);
    aboveBaseline = std::max(aboveBaseline, b);
    column++;
    l = l->nextTree();
  }
  return aboveBaseline + underBaseline;
}

KDCoordinate Grid::height(KDFont::Size font) const {
  KDCoordinate totalHeight = 0;
  for (int row = 0; row < numberOfRows(); row++) {
    totalHeight += rowHeight(row, font);
  }
  totalHeight += numberOfRows() > 0
                     ? (numberOfRows() - 1) * verticalGridEntryMargin(font)
                     : 0;
  return totalHeight;
}

KDCoordinate Grid::columnWidth(int column, KDFont::Size font) const {
  KDCoordinate columnWidth = 0;
  int childIndex = column;
  int lastIndex = (numberOfRows() - 1) * numberOfColumns() + column;
  for (const Tree* l : children()) {
    if (childIndex % numberOfColumns() == column) {
      columnWidth = std::max(columnWidth, Render::Width(l));
      if (childIndex >= lastIndex) {
        break;
      }
    }
    childIndex++;
  }
  return columnWidth;
}

KDCoordinate Grid::width(KDFont::Size font) const {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < numberOfColumns(); j++) {
    totalWidth += columnWidth(j, font);
  }
  totalWidth += numberOfColumns() > 0
                    ? (numberOfColumns() - 1) * horizontalGridEntryMargin(font)
                    : 0;
  return totalWidth;
}

bool Grid::isColumnOrRowEmpty(bool column, int index) const {
  assert(index >= 0 && index < (column ? numberOfColumns() : numberOfRows()));
  for (int i = 0; const Tree* l : children()) {
    if ((column && i > index + (numberOfRows() - 1) * numberOfColumns()) ||
        (!column && i >= (index + 1) * numberOfColumns())) {
      break;
    }
    if ((!column || i % numberOfColumns() == index) &&
        !RackLayout::IsEmpty(l)) {
      return false;
    }
    i++;
  }
  return true;
}

void Grid::addEmptyRowOrColumn(bool column, EmptyRectangle::Color color) {
  /* addChildAtIndexInPlace messes with the number of rows to keep it consistent
   * with the number of children */
  int previousNumberOfChildren = numberOfChildren();
  int previousNumberOfLines = column ? numberOfColumns() : numberOfRows();
  int otherNumberOfLines = column ? numberOfRows() : numberOfColumns();
  for (int i = 0; i < otherNumberOfLines; i++) {
    Tree* h = KRackL()->clone();
#if 0
    h.setEmptyColor(color);
#endif
    int index = column ? (i + 1) * (previousNumberOfLines + 1) - 1
                       : previousNumberOfChildren;
    child(index)->moveTreeBeforeNode(h);
  }
  if (column) {
    setNumberOfColumns(previousNumberOfLines + 1);
  } else {
    setNumberOfRows(previousNumberOfLines + 1);
  }
}

void Grid::colorGrayEmptyLayoutsInYellowInColumnOrRow(bool column,
                                                      int lineIndex) {
#if 0
  int childIndex = lineIndex * (column ? 1 : numberOfColumns());
  int startIndex = childIndex;
  int maxIndex =
      column ? (numberOfRows() - 1 - static_cast<int>(!numberOfRowsIsFixed())) *
                       numberOfColumns() +
                   lineIndex
             : lineIndex * numberOfColumns() + numberOfColumns() - 1 -
                   static_cast<int>(!numberOfColumnsIsFixed());
  for (LayoutNode *lastLayoutOfLine : childrenFromIndex(startIndex)) {
    if (childIndex > maxIndex) {
      break;
    }
    if ((!column || childIndex % numberOfColumns() == lineIndex) &&
        lastLayoutOfLine->isEmpty()) {
      assert(lastLayoutOfLine->isHorizontal());
      static_cast<HorizontalLayoutNode *>(lastLayoutOfLine)
          ->setEmptyColor(EmptyRectangle::Color::Yellow);
    }
    childIndex++;
  }
#endif
}

}  // namespace PoincareJ
