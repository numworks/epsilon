#include <escher/table_view.h>
#include <escher/metric.h>

extern "C" {
#include <assert.h>
}
#include <algorithm>

namespace Escher {

TableView::TableView(TableViewDataSource * dataSource, ScrollViewDataSource * scrollDataSource) :
  ScrollView(&m_contentView, scrollDataSource),
  m_contentView(this, dataSource, 0, Metric::CellSeparatorThickness)
{
}

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
  int lastVisibleRow = std::max(firstVisibleRow + numberOfDisplayableRows(), m_contentView.dataSource()->numberOfRows() - 1);

  for (int j = firstVisibleRow; j <= lastVisibleRow; j++) {
    reloadCellAtLocation(column, j);
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

TableView::ContentView::ContentView(TableView * tableView, TableViewDataSource * dataSource, KDCoordinate horizontalCellOverlap, KDCoordinate verticalCellOverlap) :
  View(),
  m_tableView(tableView),
  m_dataSource(dataSource),
  m_horizontalCellOverlap(horizontalCellOverlap),
  m_verticalCellOverlap(verticalCellOverlap)
{}

KDRect TableView::ContentView::cellFrame(int i, int j) const {
  KDCoordinate columnWidth = m_dataSource->columnWidth(i);
  KDCoordinate rowHeight = m_dataSource->rowHeight(j);
  if (columnWidth == 0 || rowHeight == 0) {
    return KDRectZero;
  }
  if (columnWidth == KDCOORDINATE_MAX) { // For ListViewDataSource
    columnWidth = m_tableView->maxContentWidthDisplayableWithoutScrolling();
  }
  return KDRect(
    m_dataSource->cumulatedWidthFromIndex(i), m_dataSource->cumulatedHeightFromIndex(j),
    columnWidth + m_horizontalCellOverlap,
    m_dataSource->rowHeight(j) + m_verticalCellOverlap
  );
}

KDCoordinate TableView::ContentView::width() const {
  int result = m_dataSource->cumulatedWidthFromIndex(m_dataSource->numberOfColumns())+m_horizontalCellOverlap;
  // handle the case of list: cumulatedWidthFromIndex() = KDCOORDINATE_MAX
  return result == KDCOORDINATE_MAX ? m_tableView->maxContentWidthDisplayableWithoutScrolling() : result;
}

void TableView::ContentView::reloadCellAtLocation(int i, int j, bool forceSetFrame) {
  HighlightCell * cell = cellAtLocation(i, j);
  if (cell) {
    m_dataSource->willDisplayCellAtLocation(cell, i, j);
    if (forceSetFrame) {
      cell->setFrame(cellFrame(i,j), true);
    }
  }
}

int TableView::ContentView::typeOfSubviewAtIndex(int index) const {
  assert(index >= 0);
  int i = absoluteColumnNumberFromSubviewIndex(index);
  int j = absoluteRowNumberFromSubviewIndex(index);
  int type = m_dataSource->typeAtLocation(i, j);
  return type;
}

int TableView::ContentView::typeIndexFromSubviewIndex(int index, int type) const {
  int typeIndex = 0;
  for (int k = 0; k < index; k++) {
    if (typeOfSubviewAtIndex(k) == type) {
      typeIndex++;
    }
  }
  assert(typeIndex < m_dataSource->reusableCellCount(type));
  return typeIndex;
}

HighlightCell * TableView::ContentView::cellAtLocation(int x, int y) {
  int relativeX = x-columnsScrollingOffset();
  int relativeY = y-rowsScrollingOffset();
  if (relativeY < 0 || relativeY >= numberOfDisplayableRows() || relativeX < 0 || relativeX >= numberOfDisplayableColumns()) {
    return nullptr;
  }
  int type = m_dataSource->typeAtLocation(x, y);
  int index = relativeY*numberOfDisplayableColumns()+relativeX;
  int typeIndex = typeIndexFromSubviewIndex(index, type);
  return m_dataSource->reusableCell(typeIndex, type);
}

View * TableView::ContentView::subview(int index) {
  /* This is a hack: the redrawing routine tracks a rectangle which has to be
   * redrawn. Thereby, the union of the rectangles that need to be redrawn
   * sometimes covers areas that are uselessly redrawn. We reverse the order of
   * subviews when redrawing the TableView to make it more likely to uselessly
   * redraw the top left cells rather than the bottom right cells. Due to the
   * screen driver specifications, blinking is less visible at the top left
   * corner than at the bottom right. */
  return View::subview(numberOfSubviews() - 1 - index);
}

View * TableView::ContentView::subviewAtIndex(int index) {
  int type = typeOfSubviewAtIndex(index);
  int typeIndex = typeIndexFromSubviewIndex(index, type);
  return m_dataSource->reusableCell(typeIndex, type);
}

void TableView::ContentView::layoutSubviews(bool force, bool updateCellContent) {
  /* The number of subviews might change during the layouting so it needs to be
   * recomputed at each step of the for loop. */
  for (int index = 0; index < numberOfSubviews(); index++) {
    View * cell = subviewAtIndex(index);
    int i = absoluteColumnNumberFromSubviewIndex(index);
    int j = absoluteRowNumberFromSubviewIndex(index);
    if (updateCellContent) {
      m_dataSource->willDisplayCellAtLocation(static_cast<HighlightCell *>(cell), i, j);
    }
    /* Cell's content might change and fit in the same frame. LayoutSubviews
     * must be called on each cells even with an unchanged frame. */
    cell->setFrame(cellFrame(i,j), true);
  }
}

int TableView::ContentView::numberOfDisplayableRows() const {
  int rowOffset = rowsScrollingOffset();
  int cumulatedHeight = m_tableView->bounds().height() + (m_tableView->contentOffset().y() - m_tableView->topMargin());
  int cumulatedRowIndex = m_dataSource->indexFromCumulatedHeight(cumulatedHeight);
  return std::min(m_dataSource->numberOfRows(), cumulatedRowIndex + 1) - rowOffset;
}

int TableView::ContentView::numberOfDisplayableColumns() const {
  int columnOffset = columnsScrollingOffset();
  int cumulatedWidth = m_tableView->bounds().width() + m_tableView->contentOffset().x() - m_tableView->leftMargin();
  int cumulatedColumnIndex = m_dataSource->indexFromCumulatedWidth(cumulatedWidth);
  return std::min(m_dataSource->numberOfColumns(), cumulatedColumnIndex + 1) - columnOffset;
}

int TableView::ContentView::rowsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate invisibleHeight = std::max(m_tableView->contentOffset().y() - m_tableView->topMargin(), 0);
  return m_dataSource->indexFromCumulatedHeight(invisibleHeight);
}

int TableView::ContentView::columnsScrollingOffset() const {
  /* Here, we want to translate the offset at which our tableView is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate invisibleWidth = std::max(m_tableView->contentOffset().x() - m_tableView->leftMargin(), 0);
  return m_dataSource->indexFromCumulatedWidth(invisibleWidth);
}

}
