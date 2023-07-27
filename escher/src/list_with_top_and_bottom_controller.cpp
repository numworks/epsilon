#include <escher/list_with_top_and_bottom_controller.h>

namespace Escher {

int ListWithTopAndBottomDataSource::numberOfRows() const {
  return m_innerDataSource->numberOfRows() + hasTopView() + hasBottomView();
}

void ListWithTopAndBottomDataSource::initCellsAvailableWidth(TableView* view) {
  m_innerDataSource->initCellsAvailableWidth(view);
  StandardMemoizedListViewDataSource::initCellsAvailableWidth(view);
}

KDCoordinate ListWithTopAndBottomDataSource::separatorBeforeRow(int index) {
  assert(0 <= index && index < numberOfRows());
  if (hasTopView() && index == 0) {
    return 0;
  }
  if ((hasTopView() && index == 1) ||
      (hasBottomView() && index == numberOfRows() - 1)) {
    return Metric::CommonLargeMargin;
  }
  assert(index >= hasTopView());
  return m_innerDataSource->separatorBeforeRow(index - hasTopView());
}

HighlightCell* ListWithTopAndBottomDataSource::reusableCell(int index,
                                                            int type) {
  if (type == k_topCellType) {
    assert(hasTopView() && index == 0);
    return &m_topCell;
  }
  if (type == k_bottomCellType) {
    assert(hasBottomView() && index == 0);
    return &m_bottomCell;
  }
  assert(0 <= index &&
         index < m_innerDataSource->reusableCellCount(type - k_cellTypeOffset));
  return m_innerDataSource->reusableCell(index, type - k_cellTypeOffset);
}

int ListWithTopAndBottomDataSource::reusableCellCount(int type) {
  if (type == k_topCellType || type == k_bottomCellType) {
    return 1;
  }
  return m_innerDataSource->reusableCellCount(type - k_cellTypeOffset);
}

KDCoordinate ListWithTopAndBottomDataSource::nonMemoizedRowHeight(int j) {
  int type = typeAtRow(j);
  if (type == k_topCellType) {
    return m_topCell.minimalSizeForOptimalDisplay().height();
  }
  if (type == k_bottomCellType) {
    return m_bottomCell.minimalSizeForOptimalDisplay().height();
  }
  assert(j >= hasTopView());
  return m_innerDataSource->nonMemoizedRowHeight(j - hasTopView());
}

void ListWithTopAndBottomDataSource::fillCellForRow(HighlightCell* cell,
                                                    int index) {
  int type = typeAtRow(index);
  if (type == k_topCellType || type == k_bottomCellType) {
    return;
  }
  m_innerDataSource->fillCellForRow(cell, index - hasTopView());
}

int ListWithTopAndBottomDataSource::typeAtRow(int index) const {
  assert(0 <= index && index < numberOfRows());
  if (hasTopView() && index == 0) {
    return k_topCellType;
  }
  if (hasBottomView() && index == numberOfRows() - 1) {
    return k_bottomCellType;
  }
  assert(index >= hasTopView());
  return m_innerDataSource->typeAtRow(index - hasTopView()) + k_cellTypeOffset;
}

ListWithTopAndBottomController::ListWithTopAndBottomController(
    Responder* parentResponder, View* topView, View* bottomView)
    : SelectableViewController(parentResponder),
      m_selectableListView(this, &m_outerDataSource, this, this),
      m_outerDataSource(this, topView, bottomView) {
  m_selectableListView.setTopMargin(Metric::CommonLargeMargin);
  m_selectableListView.setBottomMargin(Metric::CommonLargeMargin);
}

void ListWithTopAndBottomController::listViewDidChangeSelectionAndDidScroll(
    SelectableListView* l, int previousRow, KDPoint previousOffset,
    bool withinTemporarySelection) {
  assert(l == &m_selectableListView);
  assert(!m_outerDataSource.hasTopView() || l->selectedRow() != 0);
  assert(!m_outerDataSource.hasBottomView() ||
         l->selectedRow() != m_outerDataSource.numberOfRows() - 1);
  if (withinTemporarySelection) {
    return;
  }
  if (m_outerDataSource.hasTopView() && l->selectedRow() == 1) {
    // Show top view
    l->scrollToCell(0);
  } else if (m_outerDataSource.hasBottomView() &&
             l->selectedRow() == m_outerDataSource.numberOfRows() - 2) {
    // Show bottom view
    l->scrollToCell(m_outerDataSource.numberOfRows() - 1);
  }
}

void ListWithTopAndBottomController::didBecomeFirstResponder() {
  resetMemoization();
  selectFirstCell();
  setOffset(KDPointZero);
  m_selectableListView.reloadData();
}

}  // namespace Escher
