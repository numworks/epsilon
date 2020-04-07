#include "selectable_table_view.h"

namespace Calculation {

CalculationSelectableTableView::CalculationSelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
 SelectableTableViewDataSource * selectionDataSource, SelectableTableViewDelegate * delegate) :
  ::SelectableTableView(parentResponder, dataSource, selectionDataSource, delegate)
{
  setVerticalCellOverlap(0);
  setMargins(0);
  setDecoratorType(ScrollView::Decorator::Type::None);
}

void CalculationSelectableTableView::scrollToCell(int i, int j) {
  if (m_contentView.bounds().height() < bounds().height()) {
    setTopMargin(bounds().height() - m_contentView.bounds().height());
  } else {
    setTopMargin(0);
  }
  ::SelectableTableView::scrollToCell(i, j);
  ScrollView::layoutSubviews();
  if (m_contentView.bounds().height() - contentOffset().y() < bounds().height()) {
    KDCoordinate contentOffsetX = contentOffset().x();
    KDCoordinate contentOffsetY = dataSource()->cumulatedHeightFromIndex(dataSource()->numberOfRows()) - maxContentHeightDisplayableWithoutScrolling();
    setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  }
}

void CalculationSelectableTableView::scrollToSubviewOfTypeOfCellAtLocation(HistoryViewCellDataSource::SubviewType subviewType, int i, int j) {
  if (dataSource()->rowHeight(j) <= bounds().height()) {
    return;
  }
  /* As we scroll, the selected calculation does not use the same history view
   * cell, thus, we want to deselect the previous used history view cell. */
  unhighlightSelectedCell();

  /* Main part of the scroll */
  KDCoordinate contentOffsetX = contentOffset().x();
  KDCoordinate contentOffsetY = dataSource()->cumulatedHeightFromIndex(j+1) - maxContentHeightDisplayableWithoutScrolling();
  if (subviewType == HistoryViewCellDataSource::SubviewType::Input) {
    if (j == 0) {
      contentOffsetY = 0;
    } else {
      contentOffsetY = dataSource()->cumulatedHeightFromIndex(j);
    }
  }
  setContentOffset(KDPoint(contentOffsetX, contentOffsetY));
  /* For the same reason, we have to rehighlight the new history view cell and
   * reselect the first responder. */
  HistoryViewCell * cell = (HistoryViewCell *)(selectedCell());
  assert(cell);
  cell->setHighlighted(true);
  Container::activeApp()->setFirstResponder(cell);
}


}
