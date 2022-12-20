#include <escher/scrollable_view.h>
#include <escher/metric.h>
#include <assert.h>
#include <algorithm>

namespace Escher {

ScrollableView::ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource) :
  Responder(parentResponder),
  ScrollView(view, dataSource)
{
  setDecoratorType(ScrollView::Decorator::Type::None);
}

bool ScrollableView::handleEvent(Ion::Events::Event & event) {
  KDPoint translation = KDPointZero;
  KDCoordinate scrollStep = Ion::Events::longPressFactor() * Metric::ScrollStep;
  if (event == Ion::Events::Left) {
    KDCoordinate movementToEdge = contentOffset().x();
    if (movementToEdge > 0) {
      translation = KDPoint(-std::min(scrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Right) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().width() - bounds().width() - contentOffset().x();
    if (movementToEdge > 0) {
      translation = KDPoint(std::min(scrollStep, movementToEdge), 0);
    }
  }
  if (event == Ion::Events::Up) {
    KDCoordinate movementToEdge = contentOffset().y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, -std::min(scrollStep, movementToEdge));
    }
  }
  if (event == Ion::Events::Down) {
    KDCoordinate movementToEdge = minimalSizeForOptimalDisplay().height() - bounds().height() - contentOffset().y();
    if (movementToEdge > 0) {
      translation = KDPoint(0, std::min(scrollStep, movementToEdge));
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
  KDCoordinate viewWidth = std::max(viewSize.width(), maxContentWidthDisplayableWithoutScrolling());
  KDCoordinate viewHeight = std::max(viewSize.height(), maxContentHeightDisplayableWithoutScrolling());
  return KDSize(viewWidth, viewHeight);
}

}
