#include "equation_list_view.h"

namespace Solver {

/* EquationListView */

EquationListView::EquationListView(Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource) :
  Responder(parentResponder),
  View(),
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
  KDCoordinate braceWidth = m_braceView.minimalSizeForOptimalDisplay().width();
  m_braceView.setSize(KDSize(braceWidth, m_listView.minimalSizeForOptimalDisplay().height()-Metric::StoreRowHeight-2*k_margin));
  m_listView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()));
  m_scrollBraceView.setFrame(KDRect(0, 0, k_braceTotalWidth, m_listView.minimalSizeForOptimalDisplay().height()-Metric::StoreRowHeight-offset().y()));
}

/* EquationListView::BraceWidth */

EquationListView::BraceView::BraceView() :
  View(),
  m_displayBrace(false)
{
}

void EquationListView::BraceView::displayBrace(bool displayBrace) {
  if (m_displayBrace != displayBrace) {
    m_displayBrace = displayBrace;
    markRectAsDirty(bounds());
  }
}

void EquationListView::BraceView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_displayBrace) {
    ctx->fillRect(bounds(), KDColorRed);
  } else {
    ctx->fillRect(bounds(), KDColorWhite);
  }
}

KDSize EquationListView::BraceView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_braceWidth, bounds().height());
}

}

