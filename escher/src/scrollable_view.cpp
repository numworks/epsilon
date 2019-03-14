#include <escher/scrollable_view.h>
#include <escher/metric.h>
#include <assert.h>

ScrollableView::ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource) :
  Responder(parentResponder),
  ScrollView(view, dataSource),
  m_manualScrollingOffset(KDPointZero)
{
  setDecoratorType(ScrollView::Decorator::Type::None);
}

bool ScrollableView::handleEvent(Ion::Events::Event event) {
  KDPoint translation = KDPointZero;
  if (event == Ion::Events::Left) {
    KDCoordinate movementToEdge = m_manualScrollingOffset.x();
    if (movementToEdge > 0) {
      translation = KDPoint(-min(Metric::ScrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Right) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().width() - bounds().width() - m_manualScrollingOffset.x();
    if (movementToEdge > 0) {
      translation = KDPoint(min(Metric::ScrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Up) {
    KDCoordinate movementToEdge = m_manualScrollingOffset.y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, -min(Metric::ScrollStep, movementToEdge));
    }
  }
  if (event == Ion::Events::Down) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().height() - bounds().height() - m_manualScrollingOffset.y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, min(Metric::ScrollStep, movementToEdge));
    }
  }
  if (translation != KDPointZero) {
    m_manualScrollingOffset = m_manualScrollingOffset.translatedBy(translation);
    setContentOffset(m_manualScrollingOffset);
    return true;
  }
  return false;
}

void ScrollableView::reloadScroll(bool forceReLayout) {
  m_manualScrollingOffset = KDPointZero;
  setContentOffset(m_manualScrollingOffset, forceReLayout);
}

KDSize ScrollableView::contentSize() const {
  KDSize viewSize = ScrollView::contentSize();
  KDCoordinate viewWidth = max(viewSize.width(), maxContentWidthDisplayableWithoutScrolling());
  KDCoordinate viewHeight = max(viewSize.height(), maxContentHeightDisplayableWithoutScrolling());
  return KDSize(viewWidth, viewHeight);
}
