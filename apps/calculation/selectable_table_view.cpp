#include "selectable_table_view.h"

namespace Calculation {

CalculationSelectableTableView::CalculationSelectableTableView(Responder * parentResponder, TableViewDataSource * dataSource,
 SelectableTableViewDelegate * delegate) :
  ::SelectableTableView(parentResponder, dataSource, 0, 0, 0, 0, delegate)
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
}

}