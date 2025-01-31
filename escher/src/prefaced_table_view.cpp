#include <escher/prefaced_table_view.h>

namespace Escher {

PrefacedTableView::PrefacedTableView(
    int prefaceRow, Responder* parentResponder,
    SelectableTableView* mainTableView, TableViewDataSource* cellsDataSource,
    SelectableTableViewDelegate* delegate,
    PrefacedTableViewDelegate* prefacedTableViewDelegate)
    : Responder(parentResponder),
      m_rowPrefaceDataSource(prefaceRow, cellsDataSource),
      m_rowPrefaceView(&m_rowPrefaceDataSource, &m_rowPrefaceDataSource),
      m_mainTableView(mainTableView),
      m_prefacedDelegate(prefacedTableViewDelegate),
      m_mainTableViewTopMargin(0),
      m_mainTableDelegate(delegate),
      m_virtualOffset(KDPointZero) {
  m_mainTableView->setParentResponder(parentResponder);
  m_mainTableView->setDelegate(this);
  m_rowPrefaceView.hideScrollBars();
}

void PrefacedTableView::setMargins(KDMargins m) {
  // Main table
  m_mainTableView->setMargins(m);
  m_mainTableViewTopMargin = m.top();
  // Row preface
  m_rowPrefaceView.setMargins({m.horizontal(), KDVerticalMargins()});
}

void PrefacedTableView::setBackgroundColor(KDColor color) {
  // Main table
  m_mainTableView->setBackgroundColor(color);
  // Row preface
  m_rowPrefaceView.setBackgroundColor(color);
}

void PrefacedTableView::setCellOverlap(KDCoordinate horizontal,
                                       KDCoordinate vertical) {
  // Main table
  m_mainTableView->setHorizontalCellOverlap(horizontal);
  m_mainTableView->setVerticalCellOverlap(vertical);
  // Row preface
  m_rowPrefaceView.setHorizontalCellOverlap(horizontal);
  m_rowPrefaceView.setVerticalCellOverlap(vertical);
}

void PrefacedTableView::resetSizeAndOffsetMemoization() {
  // Main table
  m_mainTableView->resetSizeAndOffsetMemoization();
  // Row preface
  m_rowPrefaceView.resetSizeAndOffsetMemoization();
}

void PrefacedTableView::resetContentOffset() {
  /* Since cells are shared between the table and its prefaces and since
   * TableView::layoutSubview first initSize on all cells before computing the
   * actual sizes, the preface resets could mess up the frames set by the main
   * one so call it first. */
  m_rowPrefaceView.resetScroll();
  m_mainTableView->resetScroll();
}

void PrefacedTableView::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView* t, int previousSelectedCol, int previousSelectedRow,
    KDPoint previousOffset, bool withinTemporarySelection) {
  assert(t == m_mainTableView);
  if (m_mainTableDelegate) {
    m_mainTableDelegate->tableViewDidChangeSelectionAndDidScroll(
        m_mainTableView, previousSelectedCol, previousSelectedRow,
        previousOffset, withinTemporarySelection);
  }
  if (m_mainTableView->selectedRow() == -1) {
    resetContentOffset();
  }
  layoutSubviews();
}

int PrefacedTableView::numberOfRowsAtColumn(const SelectableTableView* t,
                                            int column) {
  assert(t == m_mainTableView);
  return m_mainTableDelegate
             ? m_mainTableDelegate->numberOfRowsAtColumn(t, column)
             : SelectableTableViewDelegate::numberOfRowsAtColumn(t, column);
}

View* PrefacedTableView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_mainTableView;
    case 1:
      return &m_rowPrefaceView;
    case 2:
      return m_barDecorator.verticalBar();
    default:
      assert(index == 3);
      return m_barDecorator.horizontalBar();
  }
}

void PrefacedTableView::layoutSubviewsInRect(KDRect rect, bool force) {
  KDCoordinate rowPrefaceHeight =
      m_rowPrefaceView.minimalSizeForOptimalDisplay().height() +
      m_rowPrefaceDataSource.separatorAfterPrefaceRow();
  bool rowPrefaceIsTooLarge =
      m_prefacedDelegate &&
      rowPrefaceHeight > m_prefacedDelegate->maxRowPrefaceHeight();
  bool hideRowPreface =
      m_mainTableView->selectedRow() == -1 || rowPrefaceIsTooLarge ||
      (m_mainTableView->invisibleHeight() <=
       m_rowPrefaceDataSource.cumulatedHeightAtPrefaceRow(false));

  // Main table
  KDCoordinate prefaceHeight, topMargin;
  if (hideRowPreface) {
    topMargin = m_mainTableViewTopMargin;
    prefaceHeight = 0;
  } else {
    topMargin = 0;
    prefaceHeight = rowPrefaceHeight;
  }
  m_mainTableView->margins()->setTop(topMargin);
  setChildFrame(m_mainTableView,
                KDRect(rect.x(), rect.y() + prefaceHeight, rect.width(),
                       rect.height() - prefaceHeight),
                force);

  // Adjust scroll of main table with new frame.
  if (m_mainTableView->selectedRow() >= 0) {
    // We must see entirely the cell we are selecting.
    m_mainTableView->scrollToCell(m_mainTableView->selectedColumn(),
                                  m_mainTableView->selectedRow());
  }
  // Hide the preface row and column in main table if necessary
  bool hideColumnPreface =
      m_mainTableView->bounds().width() == bounds().width();
  m_mainTableView->translateContentOffsetBy(
      KDPoint(horizontalScrollToAddToHidePrefacesInMainTable(hideColumnPreface),
              verticalScrollToAddToHidePrefacesInMainTable(hideRowPreface)));

  // Row preface
  m_rowPrefaceView.margins()->setLeft(m_mainTableView->margins()->left());
  m_rowPrefaceView.setContentOffset(
      KDPoint(m_mainTableView->contentOffset().x(), 0));
  setChildFrame(&m_rowPrefaceView,
                KDRect(rect.x(), rect.y(), rect.width(), prefaceHeight), force);
  assert(m_rowPrefaceView.margins()->horizontal() ==
         m_mainTableView->margins()->horizontal());
  assert(m_rowPrefaceView.margins()->vertical() == KDVerticalMargins());
}

void PrefacedTableView::layoutScrollbars(bool force) {
  m_mainTableView->hideScrollBars();
  KDRect r1 = KDRectZero;
  KDRect r2 = KDRectZero;
  m_barDecorator.layoutIndicators(
      this, m_mainTableView->minimalSizeForOptimalDisplay(), m_virtualOffset,
      bounds(), &r1, &r2, force);
}

void PrefacedTableView::layoutSubviews(bool force) {
  layoutSubviewsInRect(bounds(), force);
  updateVirtualOffset();
  layoutScrollbars(force);
}

void PrefacedTableView::updateVirtualOffset() {
  m_virtualOffset = m_mainTableView->contentOffset()
                        .relativeTo(relativeChildOrigin(m_mainTableView))
                        .translatedBy(marginToAddForVirtualOffset());
  assert(m_virtualOffset.x() >= 0 && m_virtualOffset.y() >= 0);
}

KDCoordinate PrefacedTableView::verticalScrollToAddToHidePrefacesInMainTable(
    bool hideRowPreface) const {
  return hideRowPreface
             ? 0
             : std::max(
                   0, m_rowPrefaceDataSource.cumulatedHeightAtPrefaceRow(true) -
                          m_mainTableView->invisibleHeight());
}
void PrefacedTableView::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::BecameFirst) {
    App::app()->setFirstResponder(m_mainTableView);
  } else {
    Responder::handleResponderChainEvent(event);
  }
}

}  // namespace Escher
