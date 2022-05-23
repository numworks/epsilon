#include "prefaced_twice_table_view.h"

using namespace Escher;

namespace Shared {

PrefacedTwiceTableView::PrefacedTwiceTableView(int prefaceRow, int prefaceColumn, Escher::Responder * parentResponder, Escher::SelectableTableView * mainTableView, Escher::TableViewDataSource * cellsDataSource, Escher::SelectableTableViewDelegate * delegate) :
  PrefacedTableView(prefaceRow, parentResponder, mainTableView, cellsDataSource, delegate),
  m_columnPrefaceDataSource(prefaceColumn, cellsDataSource),
  m_columnPrefaceView(&m_columnPrefaceDataSource, &m_columnPrefaceDataSource),
  m_storedXMargin(0)
{
  m_columnPrefaceView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void PrefacedTwiceTableView::setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) {
  m_storedXMargin = left;
  m_columnPrefaceView.setTopMargin(top);
  m_columnPrefaceView.setBottomMargin(bottom);
  PrefacedTableView::setMargins(top, right, bottom, left);
}

void PrefacedTwiceTableView::setBackgroundColor(KDColor color) {
  m_columnPrefaceView.setBackgroundColor(color);
  PrefacedTableView::setBackgroundColor(color);
}

void PrefacedTwiceTableView::setCellOverlap(KDCoordinate horizontal, KDCoordinate vertical) {
  m_columnPrefaceView.setHorizontalCellOverlap(horizontal);
  m_columnPrefaceView.setVerticalCellOverlap(vertical);
  PrefacedTableView::setCellOverlap(horizontal, vertical);
}

View * PrefacedTwiceTableView::subviewAtIndex(int index) {
  switch (index) {
  case 0:
    return m_mainTableView;
  case 1:
    return &m_prefaceView;
  default:
    assert(index == 2);
    return &m_columnPrefaceView;
  }
}

void PrefacedTwiceTableView::layoutSubviews(bool force) {
  bool hideColumnPreface = m_mainTableView->selectedRow() == -1 || m_columnPrefaceDataSource.prefaceFullyInFrame(m_mainTableView->contentOffset().x());
  if (hideColumnPreface) {
    m_columnPrefaceView.setFrame(KDRectZero, force);
    m_mainTableView->setLeftMargin(m_storedXMargin);
    m_prefaceView.setLeftMargin(m_storedXMargin);
    layoutSubviewsInRect(bounds(), force);
  } else {
    m_columnPrefaceView.setRightMargin(m_marginDelegate ? m_marginDelegate->prefaceMargin(&m_columnPrefaceView, &m_columnPrefaceDataSource) : 0);
    m_mainTableView->setLeftMargin(0);
    m_prefaceView.setLeftMargin(0);
    KDCoordinate columnPrefaceWidth = m_columnPrefaceView.minimalSizeForOptimalDisplay().width();
    layoutSubviewsInRect(KDRect(columnPrefaceWidth, 0, bounds().width() - columnPrefaceWidth, bounds().height()), force);

    KDCoordinate rowPrefaceHeight = m_prefaceView.bounds().height();
    m_columnPrefaceDataSource.setPrefaceRow(rowPrefaceHeight == 0 ? -1 : m_prefaceDataSource.prefaceRow());
    m_columnPrefaceView.setContentOffset(KDPoint(0, m_mainTableView->contentOffset().y() - rowPrefaceHeight + m_columnPrefaceView.topMargin() - m_mainTableView->topMargin()));
    m_columnPrefaceView.setFrame(KDRect(0, 0, columnPrefaceWidth, bounds().height()), force);
  }
}

}
