#include "multiple_boxes_view.h"
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Statistics {

MultipleBoxesView::MultipleBoxesView(Store * store, int * selectedBoxCalculation) :
  MultipleDataView(store),
  m_boxView1(store, 0, selectedBoxCalculation),
  m_boxView2(store, 1, selectedBoxCalculation),
  m_boxView3(store, 2, selectedBoxCalculation),
  m_axisView(store)
{
  static_assert(MultipleBoxesView::BoxToBoxMargin(2) >= BoxView::BoxVerticalMargin() && MultipleBoxesView::BoxToBoxMargin(3) >= BoxView::BoxVerticalMargin(), "BoxToBoxMargin() should be bigger than BoxVerticalMargin().");
}

BoxView *  MultipleBoxesView::dataViewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  BoxView * views[] = {&m_boxView1, &m_boxView2, &m_boxView3};
  return views[index];
}

int MultipleBoxesView::seriesOfSubviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews() - 1);
  return static_cast<BoxView *>(subviewAtIndex(index))->series();
}

void MultipleBoxesView::layoutDataSubviews(bool force) {
  int numberOfDataSubviews = m_store->numberOfValidSeries();
  assert(numberOfDataSubviews > 0);
  KDCoordinate bannerHeight = bannerFrame().height();
  KDCoordinate boxYPosition = TopToFirstBoxMargin(numberOfDataSubviews);
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsValid(i)) {
      // Add vertical margins to box layout. Boxes layouts may overlap.
      KDRect frame = KDRect(0, boxYPosition - BoxView::BoxVerticalMargin(), bounds().width(), BoxView::BoxFrameHeight(numberOfDataSubviews));
      dataViewAtIndex(i)->setFrame(frame, force);
      boxYPosition += BoxView::BoxHeight(numberOfDataSubviews) + BoxToBoxMargin(numberOfDataSubviews);
    }
  }
  // Remove BoxToBoxMargin on last box
  boxYPosition -= BoxToBoxMargin(numberOfDataSubviews);
  assert(bounds().height() >= boxYPosition + k_axisViewHeight + bannerHeight);
  // Layout the axis right above the banner
  m_axisView.setFrame(KDRect(0, bounds().height() - bannerHeight - k_axisViewHeight, bounds().width(), k_axisViewHeight), force);
}

void MultipleBoxesView::reload() {
  MultipleDataView::reload();
  m_axisView.reload();
}

bool MultipleBoxesView::moveSelectionHorizontally(int series, int deltaIndex) {
  assert(deltaIndex != 0);
  BoxView * view = dataViewAtIndex(series);
  if (view->canIncrementSelectedCalculation(deltaIndex)) {
    // Mark rect as dirty in parent's view to also redraw the background
    markRectAsDirty(view->selectedCalculationRect());
    view->incrementSelectedCalculation(deltaIndex);
    markRectAsDirty(view->selectedCalculationRect());
    return true;
  }
  return false;
}

int MultipleBoxesView::numberOfSubviews() const {
  return MultipleDataView::numberOfSubviews() + 1; // +1 for the axis view
}

View *  MultipleBoxesView::subviewAtIndex(int index) {
  if (index == numberOfSubviews() - 1) {
    return &m_axisView;
  }
  return MultipleDataView::subviewAtIndex(index);
}

void MultipleBoxesView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate bannerHeight = bannerFrame().height();
  /* Background is filled here instead of in each subviews because the boxes
   * layout can overlap, and should not draw over this each others. */
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height() - bannerHeight - k_axisViewHeight), KDColorWhite);
  MultipleDataView::drawRect(ctx, rect);
}

void MultipleBoxesView::changeDataViewSelection(int index, bool select) {
  dataViewAtIndex(index)->selectMainView(select);
  // Mark rect as dirty in parent's view to also redraw the background
  markRectAsDirty(dataViewAtIndex(index)->reloadRect());
}

}
