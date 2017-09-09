#include "selectable_table_view.h"

namespace Calculation {

CalculationSelectableTableView::CalculationSelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
 SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  ::SelectableTableView(parentResponder, dataSource, 0, 0, 0, 0, 0, 0, selectionDataSource, delegate, false)
{
}

void CalculationSelectableTableView::scrollToCell(int i, int j) {
  ::SelectableTableView::scrollToCell(i, j);
  if (m_contentView.bounds().height() < bounds().height()) {
    m_topMargin = bounds().height() - m_contentView.bounds().height();
  } else {
    m_topMargin = 0;
  }
  ScrollView::layoutSubviews();
  if (m_contentView.bounds().height() - contentOffset().y() < bounds().height()) {
    KDCoordinate contentOffsetX = contentOffset().x();
    KDCoordinate contentOffsetY = dataSource()->cumulatedHeightFromIndex(dataSource()->numberOfRows()) - maxContentHeightDisplayableWithoutScrolling();
    setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  }
  if (dataSource()->numberOfRows() > j && dataSource()->numberOfColumns() > i && dataSource()->rowHeight(j) > bounds().height()) {
    KDCoordinate contentOffsetX = contentOffset().x();
    KDCoordinate contentOffsetY = contentOffset().y();
    if (contentOffsetY > dataSource()->cumulatedHeightFromIndex(j) && contentOffsetY > dataSource()->cumulatedHeightFromIndex(j+1)) {
      // Let's scroll the tableView to align the top of the cell to the top
      contentOffsetY = dataSource()->cumulatedHeightFromIndex(j);
    } else {
      // Let's scroll the tableView to align the bottom of the cell to the bottom
      contentOffsetY = dataSource()->cumulatedHeightFromIndex(j+1) - maxContentHeightDisplayableWithoutScrolling();
    }
    setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  }
}

void CalculationSelectableTableView::scrollToSubviewOfTypeOfCellAtLocation(HistoryViewCell::SubviewType subviewType, int i, int j) {
  if (dataSource()->rowHeight(j) <= bounds().height()) {
    return;
  }
  /* As we scroll, the selected calculation does not use the same history view
   * cell, thus, we want to deselect the previous used history view cell. */
  if (selectedRow() >= 0) {
    HighlightCell * previousCell = selectedCell();
    previousCell->setHighlighted(false);
  }
  /* Main part of the scroll */
  KDCoordinate contentOffsetX = contentOffset().x();
  KDCoordinate contentOffsetY = dataSource()->cumulatedHeightFromIndex(j+1) - maxContentHeightDisplayableWithoutScrolling();
  if (subviewType == HistoryViewCell::SubviewType::Input) {
    if (j == 0) {
      contentOffsetY = 0;
    } else {
      contentOffsetY = dataSource()->cumulatedHeightFromIndex(j);
    }
  }
  /* For the same reason, we have to rehighlight the new history view cell and
   * inform the delegate which history view cell is highlighted even if the
   * selected calculation has not changed. */
  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  HighlightCell * cell = cellAtLocation(i, j);
  cell->setHighlighted(true);
  if (m_delegate) {
    m_delegate->tableViewDidChangeSelection(this, selectedColumn(), selectedRow());
  }
}


}
