#include "equation_list_view.h"

namespace Solver {

/* EquationListView */

EquationListView::EquationListView(Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource) :
  Responder(parentResponder),
  View(),
  m_displayBrace(false),
  m_listView(this, dataSource, selectionDataSource),
  m_braceView(),
  m_scrollBraceView(&m_braceView, this)
{
  m_listView.setMargins(0);
  m_listView.setVerticalCellOverlap(0);
  m_listView.setShowsIndicators(false);
  selectionDataSource->setScrollViewDelegate(this);
  m_scrollBraceView.setMargins(k_margin, k_margin, k_margin, k_margin);
  m_scrollBraceView.setShowsIndicators(false);
  m_scrollBraceView.setBackgroundColor(KDColorWhite);
}

void EquationListView::displayBrace(bool displayBrace) {
  if (m_displayBrace != displayBrace) {
    m_displayBrace = displayBrace;
    layoutSubviews();
  }
}

void EquationListView::scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) {
  m_scrollBraceView.setContentOffset(KDPoint(0, scrollViewDataSource->offset().y()));
  layoutSubviews();
}

int EquationListView::numberOfSubviews() const {
  return 2;
}

View * EquationListView::subviewAtIndex(int index) {
  View * subviews[] = {&m_listView, &m_scrollBraceView};
  return subviews[index];
}

void EquationListView::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_listView);
}

void EquationListView::layoutSubviews() {
  m_listView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
  if (m_displayBrace) {
    KDCoordinate braceWidth = m_braceView.minimalSizeForOptimalDisplay().width();
    m_braceView.setSize(KDSize(braceWidth, m_listView.minimalSizeForOptimalDisplay().height()-Metric::StoreRowHeight-2*k_margin));
    m_scrollBraceView.setFrame(KDRect(0, 0, k_braceTotalWidth, m_listView.minimalSizeForOptimalDisplay().height()-Metric::StoreRowHeight-offset().y()));
  } else {
    m_scrollBraceView.setFrame(KDRectZero);
  }
}

/* EquationListView::BraceWidth */

void EquationListView::BraceView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorRed);
}

KDSize EquationListView::BraceView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_braceWidth, bounds().height());
}

}

