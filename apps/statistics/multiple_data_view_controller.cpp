#include "multiple_data_view_controller.h"
#include <apps/i18n.h>
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
  ViewController::viewWillAppear();
  if (*m_selectedSeriesIndex < 0 || m_store->sumOfOccurrences(*m_selectedSeriesIndex) == 0) {
    *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(0);
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
      highlightSelection();
      reloadBannerView();
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
      highlightSelection();
    } else {
      Container::activeApp()->setFirstResponder(tabController());
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

void MultipleDataViewController::didEnterResponderChain(Responder * firstResponder) {
  assert(*m_selectedSeriesIndex >= 0);
  multipleDataView()->setDisplayBanner(true);
  multipleDataView()->selectDataView(*m_selectedSeriesIndex);
  highlightSelection();
}

void MultipleDataViewController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    assert(*m_selectedSeriesIndex >= 0);
    multipleDataView()->deselectDataView(*m_selectedSeriesIndex);
    multipleDataView()->setDisplayBanner(false);
  }
}

}
