#include <escher/list_with_top_and_bottom_controller.h>

namespace Escher {

int ListWithTopAndBottomDataSource::numberOfRows() const {
  return m_innerDataSource->numberOfRows() + hasTopView() + hasBottomView();
}

void ListWithTopAndBottomDataSource::initWidth(TableView* tableView) {
  m_innerDataSource->initWidth(tableView);
  StandardMemoizedListViewDataSource::initWidth(tableView);
}

KDCoordinate ListWithTopAndBottomDataSource::separatorBeforeRow(int row) const {
  assert(0 <= row && row < numberOfRows());
  if (hasTopView() && row == 0) {
    return 0;
  }
  if ((hasTopView() && row == 1) ||
      (hasBottomView() && row == numberOfRows() - 1)) {
    return Metric::CommonLargeMargin;
  }
  assert(row >= hasTopView());
  return m_innerDataSource->separatorBeforeRow(row - hasTopView());
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

int ListWithTopAndBottomDataSource::reusableCellCount(int type) const {
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
                                                    int row) {
  int type = typeAtRow(row);
  if (type == k_topCellType || type == k_bottomCellType) {
    return;
  }
  m_innerDataSource->fillCellForRow(cell, row - hasTopView());
}

int ListWithTopAndBottomDataSource::typeAtRow(int row) const {
  assert(0 <= row && row < numberOfRows());
  if (hasTopView() && row == 0) {
    return k_topCellType;
  }
  if (hasBottomView() && row == numberOfRows() - 1) {
    return k_bottomCellType;
  }
  assert(row >= hasTopView());
  return m_innerDataSource->typeAtRow(row - hasTopView()) + k_cellTypeOffset;
}

bool ListWithTopAndBottomDataSource::canSelectCellAtRow(int row) {
  if (typeAtRow(row) == k_topCellType || typeAtRow(row) == k_bottomCellType) {
    return false;
  }
  assert(row >= hasTopView());
  return m_innerDataSource->canSelectCellAtRow(row - hasTopView());
}

bool ListWithTopAndBottomDataSource::canStoreCellAtRow(int row) {
  if (typeAtRow(row) == k_topCellType || typeAtRow(row) == k_bottomCellType) {
    return false;
  }
  assert(row >= hasTopView());
  return m_innerDataSource->canStoreCellAtRow(row - hasTopView());
}

ListWithTopAndBottomController::ListWithTopAndBottomController(
    Responder* parentResponder, View* topView, View* bottomView)
    : SelectableViewController(parentResponder),
      m_selectableListView(this, &m_outerDataSource, this, this),
      m_outerDataSource(this, topView, bottomView) {
  m_selectableListView.margins()->setVertical(
      {Metric::CommonLargeMargin, Metric::CommonLargeMargin});
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

void ListWithTopAndBottomController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < firstCellIndex()) {
      selectFirstCell();
    }
    App::app()->setFirstResponder(&m_selectableListView);
  } else {
    SelectableViewController::handleResponderChainEvent(event);
  }
}

void ListWithTopAndBottomController::viewWillAppear() {
  selectRow(-1);
  setOffset(KDPointZero);
  m_selectableListView.reloadData(false);
  ViewController::viewWillAppear();
}

}  // namespace Escher
