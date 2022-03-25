#include "multiple_data_view_controller.h"
#include <apps/i18n.h>
#include <escher/container.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Statistics {

MultipleDataViewController::MultipleDataViewController(Responder * parentResponder, Escher::Responder * tabController, Escher::ButtonRowController * header, Escher::StackViewController * stackViewController, Escher::ViewController * typeViewController, Store * store, int * selectedBarIndex, int * selectedSeriesIndex) :
    ViewController(parentResponder),
    GraphButtonRowDelegate(header, stackViewController, this, typeViewController),
    m_tabController(tabController),
    m_store(store),
    m_selectedSeriesIndex(selectedSeriesIndex),
    m_selectedBarIndex(selectedBarIndex) {
  assert(numberOfButtons(Escher::ButtonRowController::Position::Top) > 0);
}

void MultipleDataViewController::viewWillAppear() {
  ViewController::viewWillAppear();
  if (*m_selectedSeriesIndex < 0 || m_store->sumOfOccurrences(*m_selectedSeriesIndex) == 0) {
    *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(0);
  }
  header()->setSelectedButton(-1);
  reloadBannerView();
  multipleDataView()->reload();
}

bool MultipleDataViewController::handleEvent(Ion::Events::Event event) {
  if (header()->selectedButton() >= 0) {
    if (event == Ion::Events::Up) {
      header()->setSelectedButton(-1);
      Container::activeApp()->setFirstResponder(tabController());
      return true;
    }
    if (event == Ion::Events::Down) {
      header()->setSelectedButton(-1);
      multipleDataView()->setDisplayBanner(true);
      multipleDataView()->selectDataView(*m_selectedSeriesIndex);
      highlightSelection();
      reloadBannerView();
      return true;
    }
    return false;
  }
  assert(*m_selectedSeriesIndex >= 0);
  if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    if (moveSelectionVertically(direction)) {
      if (reloadBannerView()) {
        multipleDataView()->reload();
      }
      return true;
    }
  if (*m_selectedSeriesIndex >= 0 && (event == Ion::Events::Left || event == Ion::Events::Right)) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    if (moveSelectionHorizontally(direction) && reloadBannerView()) {
      multipleDataView()->reload();
    }
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

bool MultipleDataViewController::moveSelectionVertically(int direction) {
  int nextSelectedSubview = multipleDataView()->indexOfSubviewAtSeries(*m_selectedSeriesIndex) - direction;
  if (nextSelectedSubview >= m_store->numberOfValidSeries()) {
    return false;
  }
  multipleDataView()->deselectDataView(*m_selectedSeriesIndex);
  if (nextSelectedSubview < 0) {
    multipleDataView()->setDisplayBanner(false);
    header()->setSelectedButton(0);
  } else {
    *m_selectedSeriesIndex = multipleDataView()->seriesOfSubviewAtIndex(nextSelectedSubview);
    *m_selectedBarIndex = MultipleDataView::k_defaultSelectedBar;
    multipleDataView()->selectDataView(*m_selectedSeriesIndex);
    highlightSelection();
  }
  return true;
}

}
