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
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight - k_axisViewHeight)/numberOfDataSubviews;
  int displayedSubviewIndex = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsValid(i)) {
      KDRect frame = KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), subviewHeight);
      dataViewAtIndex(i)->setFrame(frame, force);
      displayedSubviewIndex++;
    }
  }
  m_axisView.setFrame(KDRect(0, displayedSubviewIndex*subviewHeight, bounds().width(), bounds().height() - bannerHeight - displayedSubviewIndex*subviewHeight), force);
}

void MultipleBoxesView::reload() {
  MultipleDataView::reload();
  m_axisView.reload();
}

bool MultipleBoxesView::moveSelectionHorizontally(int series, int deltaIndex) {
  assert(deltaIndex != 0);
  BoxView * view = dataViewAtIndex(series);
  if (view->canIncrementSelectedCalculation(deltaIndex)) {
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
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height() - bannerHeight - k_axisViewHeight), KDColorWhite);
  MultipleDataView::drawRect(ctx, rect);
}

void MultipleBoxesView::changeDataViewSelection(int index, bool select) {
  dataViewAtIndex(index)->selectMainView(select);
  markRectAsDirty(dataViewAtIndex(index)->reloadRect());
}

}
