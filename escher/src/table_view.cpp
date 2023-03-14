#include <escher/metric.h>
#include <escher/table_view.h>

extern "C" {
#include <assert.h>
}
#include <algorithm>

namespace Escher {

TableView::TableView(TableViewDataSource* dataSource,
                     ScrollViewDataSource* scrollDataSource)
    : ScrollView(&m_contentView, scrollDataSource),
      m_contentView(this, dataSource, 0, Metric::CellSeparatorThickness) {}

void TableView::initSize(KDRect rect) {
  if (bounds().width() <= 0 || bounds().height() <= 0) {
    setSize(KDSize(rect.width(), rect.height()));
    dataSource()->initCellSize(this);
  }
}

void TableView::reloadVisibleCellsAtColumn(int column) {
  // Reload visible cells of the selected column
  int firstVisibleCol = firstDisplayedColumnIndex();
  int lastVisibleCol = firstVisibleCol + numberOfDisplayableColumns();
  if (column < firstVisibleCol || column >= lastVisibleCol) {
    // Column is not visible
    return;
  }
  int firstVisibleRow = firstDisplayedRowIndex();
  int lastVisibleRow = std::max(firstVisibleRow + numberOfDisplayableRows(),
                                m_contentView.dataSource()->numberOfRows() - 1);

  for (int row = firstVisibleRow; row <= lastVisibleRow; row++) {
    reloadCellAtLocation(column, row);
  }
}

void TableView::layoutSubviews(bool force) {
  /* On the one hand, ScrollView::layoutSubviews()
   * calls setFrame(...) over m_contentView,
   * which typically calls layoutSubviews() over m_contentView.
   * However, if the frame happens to be unchanged,
   * setFrame(...) does not call layoutSubviews.
   * On the other hand, calling only m_contentView.layoutSubviews()
   * does not relayout ScrollView when the offset
   * or the content's size changes.
   * For those reasons, we call both of them explicitly.
   * Besides, one must call layoutSubviews() over
   * m_contentView first, in order to reload the table's data,
   * otherwise the table's size might be miscomputed...
   * FIXME:
   * Finally, this solution is not optimal at all since
   * layoutSubviews is called twice over m_contentView. */
  dataSource()->initCellSize(this);
  m_contentView.layoutSubviews(force, true);
  ScrollView::layoutSubviews(force);
}

/* TableView::ContentView */

TableView::ContentView::ContentView(TableView* tableView,
                                    TableViewDataSource* dataSource,
                                    KDCoordinate horizontalCellOverlap,
                                    KDCoordinate verticalCellOverlap)
    : View(),
      m_tableView(tableView),
      m_dataSource(dataSource),
      m_horizontalCellOverlap(horizontalCellOverlap),
      m_verticalCellOverlap(verticalCellOverlap) {}

KDRect TableView::ContentView::cellFrame(int col, int row) const {
  KDCoordinate columnWidth = m_dataSource->columnWidth(col);
  KDCoordinate rowHeight = m_dataSource->rowHeight(row);
  if (columnWidth == 0 || rowHeight == 0) {
    return KDRectZero;
  }
  if (columnWidth == KDCOORDINATE_MAX) {  // For ListViewDataSource
    columnWidth = m_tableView->maxContentWidthDisplayableWithoutScrolling();
  }
  return KDRect(m_dataSource->cumulatedWidthBeforeIndex(col),
                m_dataSource->cumulatedHeightBeforeIndex(row),
                columnWidth + m_horizontalCellOverlap,
                rowHeight + m_verticalCellOverlap);
}

KDCoordinate TableView::ContentView::width() const {
  int result =
      m_dataSource->cumulatedWidthBeforeIndex(m_dataSource->numberOfColumns()) +
      m_horizontalCellOverlap;
  // handle the case of list: cumulatedWidthBeforeIndex() = KDCOORDINATE_MAX
  return result == KDCOORDINATE_MAX
             ? m_tableView->maxContentWidthDisplayableWithoutScrolling()
             : result;
}

void TableView::ContentView::reloadCellAtLocation(int col, int row,
                                                  bool forceSetFrame) {
  HighlightCell* cell = cellAtLocation(col, row);
  if (cell) {
    m_dataSource->willDisplayCellAtLocation(cell, col, row);
    if (forceSetFrame) {
      setChildFrame(cell, cellFrame(col, row), true);
    }
  }
}

int TableView::ContentView::typeOfSubviewAtIndex(int index) const {
  assert(index >= 0);
  int col = absoluteColumnIndexFromSubviewIndex(index);
  int row = absoluteRowIndexFromSubviewIndex(index);
  int type = m_dataSource->typeAtLocation(col, row);
  return type;
}

int TableView::ContentView::typeIndexFromSubviewIndex(int index,
                                                      int type) const {
  int typeIndex = 0;
  for (int k = 0; k < index; k++) {
    if (typeOfSubviewAtIndex(k) == type) {
      typeIndex++;
    }
  }
  assert(typeIndex < m_dataSource->reusableCellCount(type));
  return typeIndex;
}

HighlightCell* TableView::ContentView::cellAtLocation(int col, int row) {
  int relativeColumn = col - columnsScrollingOffset();
  int relativeRow = row - rowsScrollingOffset();
  if (relativeRow < 0 || relativeRow >= numberOfDisplayableRows() ||
      relativeColumn < 0 || relativeColumn >= numberOfDisplayableColumns()) {
    return nullptr;
  }
  int type = m_dataSource->typeAtLocation(col, row);
  int index = relativeRow * numberOfDisplayableColumns() + relativeColumn;
  int typeIndex = typeIndexFromSubviewIndex(index, type);
  return m_dataSource->reusableCell(typeIndex, type);
}

View* TableView::ContentView::subviewAtIndex(int index) {
  /* This is a hack: the redrawing routine tracks a rectangle which has to be
   * redrawn. Thereby, the union of the rectangles that need to be redrawn
   * sometimes covers areas that are uselessly redrawn. We reverse the order of
   * subviews when redrawing the TableView to make it more likely to uselessly
   * redraw the top left cells rather than the bottom right cells. Due to the
   * screen driver specifications, blinking is less visible at the top left
   * corner than at the bottom right. */
  return static_cast<View*>(
      reusableCellAtIndex(numberOfSubviews() - 1 - index));
}

HighlightCell* TableView::ContentView::reusableCellAtIndex(int index) {
  int type = typeOfSubviewAtIndex(index);
  int typeIndex = typeIndexFromSubviewIndex(index, type);
  return m_dataSource->reusableCell(typeIndex, type);
}

void TableView::ContentView::layoutSubviews(bool force,
                                            bool updateCellContent) {
  /* The number of cells might change during the layouting so it needs to be
   * recomputed at each step of the for loop. */
  for (int index = 0; index < numberOfDisplayableCells(); index++) {
    HighlightCell* cell = reusableCellAtIndex(index);
    int col = absoluteColumnIndexFromSubviewIndex(index);
    int row = absoluteRowIndexFromSubviewIndex(index);
    assert(cellAtLocation(col, row) == cell);
    if (updateCellContent) {
      m_dataSource->willDisplayCellAtLocation(cell, col, row);
    }
    /* Cell's content might change and fit in the same frame. LayoutSubviews
     * must be called on each cells even with an unchanged frame. */
    setChildFrame(cell, cellFrame(col, row), true);
  }
}

int TableView::ContentView::numberOfDisplayableRows() const {
  int rowOffset = rowsScrollingOffset();
  int cumulatedHeightOfLastVisiblePixel =
      m_tableView->bounds().height() + invisibleHeight() - 1;
  int cumulatedRowIndexOfLastVisiblePixel =
      m_dataSource->indexAfterCumulatedHeight(
          cumulatedHeightOfLastVisiblePixel);
  return std::min(m_dataSource->numberOfRows(),
                  cumulatedRowIndexOfLastVisiblePixel + 1) -
         rowOffset;
}

int TableView::ContentView::numberOfDisplayableColumns() const {
  int columnOffset = columnsScrollingOffset();
  int cumulatedWidthOfLastVisiblePixel =
      m_tableView->bounds().width() + invisibleWidth() - 1;
  int cumulatedColumnIndexOfLastVisiblePixel =
      m_dataSource->indexAfterCumulatedWidth(cumulatedWidthOfLastVisiblePixel);
  return std::min(m_dataSource->numberOfColumns(),
                  cumulatedColumnIndexOfLastVisiblePixel + 1) -
         columnOffset;
}

}  // namespace Escher
