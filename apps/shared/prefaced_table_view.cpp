#include "prefaced_table_view.h"

using namespace Escher;

namespace Shared {

  PrefacedTableView::PrefacedTableView(int prefaceColumn, Responder * parentResponder, SelectableTableView * mainTableView, TableViewDataSource * cellsDataSource, SelectableTableViewDelegate * delegate) :
  Escher::Responder(parentResponder),
  m_prefaceDataSource(prefaceColumn, cellsDataSource),
  m_prefaceView(&m_prefaceDataSource, &m_prefaceDataSource),
  m_mainTableView(mainTableView),
  m_mainTableDelegate(delegate),
  m_marginDelegate(nullptr),
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

  m_prefaceView.setTopMargin(top);
  m_prefaceView.setBottomMargin(bottom);
}

void PrefacedTableView::setBackgroundColor(KDColor color) {
  m_mainTableView->setBackgroundColor(color);
  m_prefaceView.setBackgroundColor(color);
}

void PrefacedTableView::tableViewDidChangeSelection(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelection(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
}

void PrefacedTableView::tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  /* FIXME: We need a +1 to align the TableView with the SelectableTableView.
   * Find out why! */
  m_prefaceView.setContentOffset(KDPoint(0, m_mainTableView->contentOffset().y() + 1));
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelectionAndDidScroll(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  }
  layoutSubviews();
}

void PrefacedTableView::layoutSubviews(bool force) {
  KDPoint mainContentOffset = m_mainTableView->contentOffset();
  bool hidePreface = m_prefaceDataSource.prefaceFullyInFrame(mainContentOffset.x()) || m_mainTableView->selectedRow() == -1;
  if (hidePreface) {
    m_mainTableView->setLeftMargin(m_storedMargin);
    m_mainTableView->setFrame(bounds(), force);
    m_prefaceView.setRightMargin(0);
    m_prefaceView.setFrame(KDRectZero, force);
  } else {
    m_prefaceView.setRightMargin(m_marginDelegate ? m_marginDelegate->prefaceMargin(&m_prefaceView) : 0);
    KDCoordinate prefaceWidth = m_prefaceView.minimalSizeForOptimalDisplay().width();
    if (m_prefaceView.bounds().isEmpty()) {
      m_storedMargin = m_mainTableView->leftMargin();
      m_mainTableView->setLeftMargin(0);
      m_mainTableView->setContentOffset(mainContentOffset.translatedBy(KDPoint(prefaceWidth - m_storedMargin, 0)));
    }
    m_mainTableView->setFrame(KDRect(prefaceWidth, 0, bounds().width() - prefaceWidth, bounds().height()), force);
    m_prefaceView.setFrame(KDRect(0, 0, prefaceWidth, bounds().height()), force);
  }
}

}
