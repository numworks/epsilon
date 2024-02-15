#include "grid.h"

#include "k_tree.h"
#include "layout_cursor.h"
#include "rack_layout.h"
#include "render.h"

namespace PoincareJ {

bool Grid::isEditing() const {
  if (SharedEditionPool->contains(this)) {
    return true;
  }
  if (!RackLayout::s_layoutCursor) {
    return false;
  }
  // TODO isEditing is called a lot, is nextTree too expensive ?
  Tree* cursorNode = RackLayout::s_layoutCursor->cursorNode();
  return this <= cursorNode && cursorNode < nextTree();
}

const Rack* Grid::childAt(uint8_t col, uint8_t row) const {
  return child(row * numberOfColumns() + col);
}

bool Grid::childIsPlaceholder(int index) const {
  return childIsBottomOfGrid(index) ||
         (childIsRightOfGrid(index) &&
          (!isPiecewiseLayout() || (childIsInLastNonGrayRow(index) &&
                                    RackLayout::IsEmpty(child(index)))));
}

Rack* Grid::willFillEmptyChildAtIndex(int childIndex) {
  assert(isEditing());
  bool isBottomOfGrid = childIsBottomOfGrid(childIndex);
  bool isRightOfGrid = childIsRightOfGrid(childIndex);
  int column = columnAtChildIndex(childIndex);
  int row = rowAtChildIndex(childIndex);
  if (isRightOfGrid && !numberOfColumnsIsFixed()) {
    addEmptyColumn();
  }
  if (isBottomOfGrid && !numberOfRowsIsFixed()) {
    addEmptyRow();
  }
  return childAt(column, row);
}

int Grid::removeTrailingEmptyRowOrColumnAtChildIndex(int childIndex) {
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
    Tree::child(index * nbColumns)->removeTree();
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
    Tree::child(i)->removeTree();
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
  return columnAtChildIndex(index) == numberOfColumns() - 2;
}

bool Grid::childIsInLastNonGrayRow(int index) const {
  assert(index >= 0 && index < numberOfRows() * numberOfColumns());
  return rowAtChildIndex(index) == numberOfRows() - 2;
}

int Grid::rowAtChildIndex(int index) const { return index / numberOfColumns(); }

int Grid::columnAtChildIndex(int index) const {
  return index % numberOfColumns();
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
  const Rack* child = childAt(0, row);
  for (int column = 0; column < numberOfColumns(); column++) {
    rowBaseline = std::max(rowBaseline, Render::Baseline(child));
    child = child->nextTree();
  }
  return rowBaseline;
}

KDCoordinate Grid::rowHeight(int row, KDFont::Size font) const {
  KDCoordinate underBaseline = 0;
  KDCoordinate aboveBaseline = 0;
  for (int column = 0; column < numberOfColumns(); column++) {
    const Rack* r = childAt(column, row);
    KDCoordinate b = Render::Baseline(r);
    underBaseline =
        std::max<KDCoordinate>(underBaseline, Render::Height(r) - b);
    aboveBaseline = std::max(aboveBaseline, b);
  }
  return aboveBaseline + underBaseline;
}

KDCoordinate Grid::height(KDFont::Size font) const {
  return size(font).height();
}

KDCoordinate Grid::columnWidth(int column, KDFont::Size font) const {
  // TODO what is the complexity of this ?
  KDCoordinate columnWidth = 0;
  for (int row = 0; row < numberOfRows(); row++) {
    columnWidth = std::max(columnWidth, Render::Width(childAt(column, row)));
  }
  return columnWidth;
}

KDCoordinate Grid::width(KDFont::Size font) const { return size(font).width(); }

void Grid::computePositions(KDFont::Size font, KDCoordinate* columns,
                            KDCoordinate* rows) const {
  for (int c = 0; c < numberOfColumns(); c++) {
    columns[c] = 0;
  }
  for (int r = 0; r < numberOfRows(); r++) {
    rows[r] = 0;
  }
  KDCoordinate underBaseline = 0;
  KDCoordinate aboveBaseline = 0;
  for (int i = 0; const Tree* childTree : children()) {
    const Rack* child = Rack::From(childTree);
    KDSize size = Render::Size(child);
    KDCoordinate baseline = Render::Baseline(child);
    int c = columnAtChildIndex(i);
    int r = rowAtChildIndex(i);
    if (c == 0) {
      underBaseline = aboveBaseline = 0;
    }
    aboveBaseline = std::max(aboveBaseline, baseline);
    underBaseline =
        std::max<KDCoordinate>(underBaseline, size.height() - baseline);
    columns[c] = std::max(columns[c], size.width());
    rows[r] = std::max<KDCoordinate>(rows[r], aboveBaseline + underBaseline);
    i++;
  }
  // Accumulate and add margins
  for (int c = 1; c < numberOfColumns(); c++) {
    columns[c] += columns[c - 1] + horizontalGridEntryMargin(font);
  }
  for (int r = 1; r < numberOfRows(); r++) {
    rows[r] += rows[r - 1] + verticalGridEntryMargin(font);
  }
}

KDSize Grid::size(KDFont::Size font) const {
  int rows = numberOfRows();
  int columns = numberOfColumns();
  bool editing = isEditing();
  KDCoordinate columsCumulatedWidth[columns];
  KDCoordinate rowCumulatedHeight[rows];
  computePositions(font, columsCumulatedWidth, rowCumulatedHeight);
  KDSize size(columsCumulatedWidth[columns - 1 -
                                   (!numberOfColumnsIsFixed() && !isEditing())],
              rowCumulatedHeight[rows - 1 - !editing]);
  return size;
}

bool Grid::isColumnOrRowEmpty(bool column, int index) const {
  assert(index >= 0 && index < (column ? numberOfColumns() : numberOfRows()));
  int number = column ? numberOfRows() : numberOfColumns();
  for (int i = 0; i < number; i++) {
    if (!RackLayout::IsEmpty(column ? childAt(index, i) : childAt(i, index))) {
      return false;
    }
  }
  return true;
}

void Grid::addEmptyRow() {
  LayoutT* last = nextTree();
  setNumberOfRows(numberOfRows() + 1);
  for (int i = 0; i < numberOfColumns(); i++) {
    last->cloneTreeBeforeNode(KRackL());
  }
}

void Grid::addEmptyColumn() {
  int oldNumberOfColumns = numberOfColumns();
  setNumberOfColumns(oldNumberOfColumns + 1);
  Tree* tree = this;
  for (int i = 0; i < numberOfRows(); i++) {
    // Skip grid (i == 0) or empty rack
    tree = tree->nextNode();
    for (int j = 0; j < oldNumberOfColumns; j++) {
      tree = tree->nextTree();
    };
    tree->cloneTreeBeforeNode(KRackL());
  }
}

}  // namespace PoincareJ
