#include "prefaced_table_view.h"

using namespace Escher;

namespace Shared {

  PrefacedTableView::PrefacedTableView(int prefaceRow, Responder * parentResponder, SelectableTableView * mainTableView, TableViewDataSource * cellsDataSource, SelectableTableViewDelegate * delegate) :
  Escher::Responder(parentResponder),
  m_prefaceDataSource(prefaceRow, cellsDataSource),
  m_prefaceView(&m_prefaceDataSource, &m_prefaceDataSource),
  m_mainTableView(mainTableView),
  m_marginDelegate(nullptr),
  m_mainTableDelegate(delegate),
  m_storedMargin(0)
{
  m_mainTableView->setParentResponder(parentResponder);
  m_mainTableView->setDelegate(this);
  m_prefaceView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void PrefacedTableView::setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) {
  m_mainTableView->setTopMargin(top);
  m_mainTableView->setRightMargin(right);
  m_mainTableView->setBottomMargin(bottom);
  m_mainTableView->setLeftMargin(left);
  m_storedMargin = top;

  m_prefaceView.setLeftMargin(left);
  m_prefaceView.setRightMargin(right);
}

void PrefacedTableView::setBackgroundColor(KDColor color) {
  m_mainTableView->setBackgroundColor(color);
  m_prefaceView.setBackgroundColor(color);
}

void PrefacedTableView::setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) {
  m_mainTableView->setHorizontalCellOverlap(horizontal);
  m_mainTableView->setVerticalCellOverlap(vertical);
  m_prefaceView.setHorizontalCellOverlap(horizontal);
  m_prefaceView.setVerticalCellOverlap(vertical);
}

void PrefacedTableView::tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelection(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
}

void PrefacedTableView::tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelectionAndDidScroll(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
  layoutSubviews();
}

void PrefacedTableView::layoutSubviewsInRect(KDRect rect, bool force) {
  bool hidePreface = m_prefaceDataSource.prefaceFullyInFrame(m_mainTableView->contentOffset().y()) || m_mainTableView->selectedRow() == -1;
  KDCoordinate prefaceHeight;

  if (hidePreface) {
    m_mainTableView->setTopMargin(m_storedMargin);
    m_mainTableView->setFrame(rect, force);
  } else {
    prefaceHeight = m_prefaceView.minimalSizeForOptimalDisplay().height();
    m_mainTableView->setTopMargin(0);
    m_mainTableView->setFrame(KDRect(rect.x(), rect.y() + prefaceHeight, rect.width(), rect.height() - prefaceHeight), force);
  }

  if (m_mainTableView->selectedRow() >= 0) {
    /* Scroll to update the content offset with the new margins. */
    m_mainTableView->scrollToCell(m_mainTableView->selectedColumn(), m_mainTableView->selectedRow());
  }

  if (hidePreface) {
    m_prefaceView.setFrame(KDRectZero, force);
  } else {
    m_prefaceView.setContentOffset(KDPoint(m_mainTableView->contentOffset().x(), 0));
    m_prefaceView.setBottomMargin(m_marginDelegate ? m_marginDelegate->prefaceMargin(&m_prefaceView) : 0);
    m_prefaceView.setFrame(KDRect(rect.x(), rect.y(), rect.width(), prefaceHeight), force);
  }
}

void PrefacedTableView::layoutSubviews(bool force) {
  layoutSubviewsInRect(bounds(), force);
}

}
