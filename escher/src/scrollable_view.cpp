#include <escher/scrollable_view.h>
#include <escher/metric.h>
#include <assert.h>

ScrollableView::ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource) :
  Responder(parentResponder),
  ScrollView(view, dataSource, 0, 0, 0, 0, false, false),
  m_manualScrolling(0)
{
}

bool ScrollableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Right && rightViewIsInvisible()) {
      KDCoordinate rightSpace = view()->minimalSizeForOptimalDisplay().width() - m_manualScrolling - bounds().width();
      KDCoordinate scrollAdd = rightSpace > Metric::ScrollStep ? Metric::ScrollStep : rightSpace;
      m_manualScrolling += scrollAdd;
      setContentOffset(KDPoint(m_manualScrolling, 0));
      return true;
  }
  if (event == Ion::Events::Left && m_manualScrolling > 0) {
    KDCoordinate leftSpace = m_manualScrolling;
    KDCoordinate scrollSubstract = leftSpace > Metric::ScrollStep ? Metric::ScrollStep : leftSpace;
    m_manualScrolling -= scrollSubstract;
    setContentOffset(KDPoint(m_manualScrolling, 0));
    return true;
  }
  return false;
}

void ScrollableView::reloadScroll() {
  m_manualScrolling = 0;
  setContentOffset(KDPoint(m_manualScrolling, 0));
}

void ScrollableView::layoutSubviews() {
  KDSize viewSize = view()->minimalSizeForOptimalDisplay();
  KDCoordinate viewWidth = viewSize.width() < bounds().width() ? bounds().width() : viewSize.width();
  KDCoordinate viewHeight = viewSize.height() < bounds().height() ? bounds().height() : viewSize.height();
  view()->setSize(KDSize(viewWidth, viewHeight));
  ScrollView::layoutSubviews();
}

bool ScrollableView::rightViewIsInvisible() {
  return view()->minimalSizeForOptimalDisplay().width() - m_manualScrolling > bounds().width();
}

