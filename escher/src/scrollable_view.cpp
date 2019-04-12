#include <escher/scrollable_view.h>
#include <escher/metric.h>
#include <assert.h>

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }
static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

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
      translation = KDPoint(-minCoordinate(Metric::ScrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Right) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().width() - bounds().width() - m_manualScrollingOffset.x();
    if (movementToEdge > 0) {
      translation = KDPoint(minCoordinate(Metric::ScrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Up) {
    KDCoordinate movementToEdge = m_manualScrollingOffset.y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, -minCoordinate(Metric::ScrollStep, movementToEdge));
    }
  }
  if (event == Ion::Events::Down) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().height() - bounds().height() - m_manualScrollingOffset.y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, minCoordinate(Metric::ScrollStep, movementToEdge));
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
  KDCoordinate viewWidth = maxCoordinate(viewSize.width(), maxContentWidthDisplayableWithoutScrolling());
  KDCoordinate viewHeight = maxCoordinate(viewSize.height(), maxContentHeightDisplayableWithoutScrolling());
  return KDSize(viewWidth, viewHeight);
}
