#include <escher/scrollable_view.h>
#include <escher/metric.h>
#include <assert.h>

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }
static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

ScrollableView::ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource) :
  Responder(parentResponder),
  ScrollView(view, dataSource)
{
  setDecoratorType(ScrollView::Decorator::Type::None);
}

bool ScrollableView::handleEvent(Ion::Events::Event event) {
  KDPoint translation = KDPointZero;
  if (event == Ion::Events::Left) {
    KDCoordinate movementToEdge = contentOffset().x();
    if (movementToEdge > 0) {
      translation = KDPoint(-minCoordinate(Metric::ScrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Right) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().width() - bounds().width() - contentOffset().x();
    if (movementToEdge > 0) {
      translation = KDPoint(minCoordinate(Metric::ScrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Up) {
    KDCoordinate movementToEdge = contentOffset().y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, -minCoordinate(Metric::ScrollStep, movementToEdge));
    }
  }
  if (event == Ion::Events::Down) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().height() - bounds().height() - contentOffset().y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, minCoordinate(Metric::ScrollStep, movementToEdge));
    }
  }
  if (translation != KDPointZero) {
    setContentOffset(contentOffset().translatedBy(translation));
    return true;
  }
  return false;
}

void ScrollableView::reloadScroll(bool forceReLayout) {
  setContentOffset(KDPointZero, forceReLayout);
}

KDSize ScrollableView::contentSize() const {
  KDSize viewSize = ScrollView::contentSize();
  KDCoordinate viewWidth = maxCoordinate(viewSize.width(), maxContentWidthDisplayableWithoutScrolling());
  KDCoordinate viewHeight = maxCoordinate(viewSize.height(), maxContentHeightDisplayableWithoutScrolling());
  return KDSize(viewWidth, viewHeight);
}
