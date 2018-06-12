#include "multiple_data_view_controller.h"
#include "../i18n.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

MultipleDataViewController::MultipleDataViewController(Responder * parentResponder, Store * store, int * selectedBarIndex, int * selectedSeriesIndex) :
  ViewController(parentResponder),
  m_store(store),
  m_selectedSeriesIndex(selectedSeriesIndex),
  m_selectedBarIndex(selectedBarIndex)
{
}

bool MultipleDataViewController::isEmpty() const {
  return m_store->isEmpty();
}

I18n::Message MultipleDataViewController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

Responder * MultipleDataViewController::defaultController() {
  return tabController();
}

void MultipleDataViewController::viewWillAppear() {
  multipleDataView()->setDisplayBanner(true);
  if (*m_selectedSeriesIndex < 0) {
    *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(0);
    multipleDataView()->selectDataView(*m_selectedSeriesIndex);
  }
  reloadBannerView();
  multipleDataView()->reload();
}

bool MultipleDataViewController::handleEvent(Ion::Events::Event event) {
  assert(*m_selectedSeriesIndex >= 0);
  if (event == Ion::Events::Down) {
    int currentSelectedSubview = multipleDataView()->indexOfSubviewAtSeries(*m_selectedSeriesIndex);
    if (currentSelectedSubview < m_store->numberOfNonEmptySeries() - 1) {
      multipleDataView()->deselectDataView(*m_selectedSeriesIndex);
      *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(currentSelectedSubview+1);
      *m_selectedBarIndex = MultipleDataView::k_defaultSelectedBar;
      multipleDataView()->selectDataView(*m_selectedSeriesIndex);
      reloadBannerView();
      app()->setFirstResponder(this);
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    int currentSelectedSubview = multipleDataView()->indexOfSubviewAtSeries(*m_selectedSeriesIndex);
    if (currentSelectedSubview > 0) {
      multipleDataView()->deselectDataView(*m_selectedSeriesIndex);
      *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(currentSelectedSubview-1);
      *m_selectedBarIndex = MultipleDataView::k_defaultSelectedBar;
      multipleDataView()->selectDataView(*m_selectedSeriesIndex);
      app()->setFirstResponder(this);
    } else {
      app()->setFirstResponder(tabController());
    }
    reloadBannerView();
    return true;
  }
  if (*m_selectedSeriesIndex >= 0 && (event == Ion::Events::Left || event == Ion::Events::Right)) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    moveSelectionHorizontally(direction);
    return true;
  }
  return false;
}

void MultipleDataViewController::didBecomeFirstResponder() {
  multipleDataView()->setDisplayBanner(true);
  if (*m_selectedSeriesIndex < 0 || m_store->sumOfOccurrences(*m_selectedSeriesIndex) == 0) {
    if (*m_selectedSeriesIndex >= 0) {
      multipleDataView()->deselectDataView(*m_selectedSeriesIndex);
    }
    *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(0);
    multipleDataView()->selectDataView(*m_selectedSeriesIndex);
    multipleDataView()->reload();
  }
}

void MultipleDataViewController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr || nextFirstResponder == tabController()) {
    if (*m_selectedSeriesIndex >= 0) {
      multipleDataView()->dataViewAtIndex(*m_selectedSeriesIndex)->selectMainView(false);
      *m_selectedSeriesIndex = -1;
      multipleDataView()->setDisplayBanner(false);
    }
  }
}

}
