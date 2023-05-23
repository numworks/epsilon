#include <escher/selectable_list_view_with_top_and_bottom_views.h>

namespace Escher {

int ListViewWithTopAndBottomViewsDataSource::numberOfRows() const {
  return m_innerDataSource->numberOfRows() + hasTopView() + hasBottomView();
}

KDCoordinate ListViewWithTopAndBottomViewsDataSource::separatorBeforeRow(
    int index) {
  assert(0 <= index && index < numberOfRows());
  if (hasTopView() && index == 0) {
    return 0;
  }
  if ((hasTopView() && index == 1) ||
      (hasBottomView() && index == numberOfRows() - 1)) {
    return Metric::TableSeparatorThickness;
  }
  assert(index >= hasTopView());
  return m_innerDataSource->separatorBeforeRow(index - hasTopView());
}

HighlightCell* ListViewWithTopAndBottomViewsDataSource::reusableCell(int index,
                                                                     int type) {
  if (type == k_topCellType) {
    assert(hasTopView() && index == 0);
    return &m_topCell;
  }
  if (type == k_bottomCellType) {
    assert(hasBottomView() && index == 0);
    return &m_bottomCell;
  }
  assert(0 <= index && index < m_innerDataSource->reusableCellCount(type));
  return m_innerDataSource->reusableCell(index, type);
}

int ListViewWithTopAndBottomViewsDataSource::reusableCellCount(int type) {
  if (type == k_topCellType || type == k_bottomCellType) {
    return 1;
  }
  return m_innerDataSource->reusableCellCount(type);
}

KDCoordinate ListViewWithTopAndBottomViewsDataSource::nonMemoizedRowHeight(
    int j) {
  int type = typeAtIndex(j);
  if (type == k_topCellType) {
    return topCellHeight();
  }
  if (type == k_bottomCellType) {
    return m_bottomCell.minimalSizeForOptimalDisplay().height();
  }
  assert(j >= hasTopView());
  return m_innerDataSource->nonMemoizedRowHeight(j - hasTopView());
}

void ListViewWithTopAndBottomViewsDataSource::willDisplayCellForIndex(
    HighlightCell* cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_topCellType || type == k_bottomCellType) {
    return;
  }
  m_innerDataSource->willDisplayCellForIndex(cell, index - hasTopView());
}

int ListViewWithTopAndBottomViewsDataSource::typeAtIndex(int index) const {
  assert(0 <= index && index < numberOfRows());
  if (hasTopView() && index == 0) {
    return k_topCellType;
  }
  if (hasBottomView() && index == numberOfRows() - 1) {
    return k_bottomCellType;
  }
  assert(index >= hasTopView());
  return m_innerDataSource->typeAtIndex(index - hasTopView());
}

SelectableListViewWithTopAndBottomViews::
    SelectableListViewWithTopAndBottomViews(Responder* parentResponder,
                                            View* topView, View* bottomView)
    : SelectableViewController(parentResponder),
      m_dataSource(this, topView, bottomView),
      m_selectableListView(this, &m_dataSource, this, this) {
  m_selectableListView.setTopMargin(Metric::CommonMargin / 2);
  m_selectableListView.setBottomMargin(Metric::CommonMargin / 2);
}

void SelectableListViewWithTopAndBottomViews::
    listViewDidChangeSelectionAndDidScroll(SelectableListView* l,
                                           int previousRow,
                                           KDPoint previousOffset,
                                           bool withinTemporarySelection) {
  assert(l == &m_selectableListView);
  assert(!m_dataSource.hasTopView() || l->selectedRow() != 0);
  assert(!m_dataSource.hasBottomView() ||
         l->selectedRow() != m_dataSource.numberOfRows() - 1);
  if (withinTemporarySelection) {
    return;
  }
  if (m_dataSource.hasTopView() && l->selectedRow() == 1) {
    // Show top view
    l->scrollToCell(0);
  } else if (m_dataSource.hasBottomView() &&
             l->selectedRow() == m_dataSource.numberOfRows() - 2) {
    // Show bottom view
    l->scrollToCell(m_dataSource.numberOfRows() - 1);
  }
}

void SelectableListViewWithTopAndBottomViews::didBecomeFirstResponder() {
  resetMemoization();
  selectFirstCell();
  m_selectableListView.reloadData();
}

}  // namespace Escher
