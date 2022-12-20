#include "data_view_controller.h"
#include <escher/container.h>
#include <assert.h>

namespace Statistics {

DataViewController::DataViewController(Escher::Responder * parentResponder, Escher::TabViewController * tabController, Escher::ButtonRowController * header, Escher::StackViewController * stackViewController, Escher::ViewController * typeViewController, Store * store) :
  ViewController(parentResponder),
  GraphButtonRowDelegate(header, stackViewController, this, typeViewController),
  m_store(store),
  m_selectedSeries(-1),
  m_selectedIndex(DataView::k_defaultSelectedIndex),
  m_tabController(tabController)
{
  assert(numberOfButtons(Escher::ButtonRowController::Position::Top) > 0);
}

void DataViewController::viewWillAppear() {
  ViewController::viewWillAppear();
  sanitizeSeriesIndex();
  dataView()->setDisplayBanner(true);
  dataView()->selectViewForSeries(m_selectedSeries);
  highlightSelection();

  /* Call children's viewWillAppear implementation after sanitizing selected
   * series index and before reloading banner and the dataView */
  viewWillAppearBeforeReload();

  header()->setSelectedButton(-1);
  reloadBannerView();
  dataView()->reload();
}

bool DataViewController::handleEvent(Ion::Events::Event & event) {
  int selectedButton = header()->selectedButton();
  if (selectedButton >= 0) {
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::Down && m_store->hasValidSeries(validSerieMethod())) {
      header()->setSelectedButton(-1);
      Escher::Container::activeApp()->setFirstResponder(this);
      dataView()->setDisplayBanner(true);
      dataView()->selectViewForSeries(m_selectedSeries);
      highlightSelection();
      reloadBannerView();
      return true;
    }
    return buttonAtIndex(selectedButton, Escher::ButtonRowController::Position::Top)->handleEvent(event);
  }
  assert(m_selectedSeries >= 0 && m_store->hasValidSeries(validSerieMethod()));
  bool isVerticalEvent = (event == Ion::Events::Down || event == Ion::Events::Up);
  if ((isVerticalEvent || event == Ion::Events::Left || event == Ion::Events::Right)) {
    int direction = (event == Ion::Events::Up || event == Ion::Events::Left) ? -1 : 1;
    if (isVerticalEvent ? moveSelectionVertically(direction) : moveSelectionHorizontally(direction)) {
      if (reloadBannerView()) {
        dataView()->reload();
      }
      return true;
    }
  }
  return false;
}

void DataViewController::didEnterResponderChain(Responder * firstResponder) {
  if (!m_store->hasValidSeries(validSerieMethod()) || !dataView()->plotViewForSeries(m_selectedSeries)->hasFocus()) {
    header()->setSelectedButton(0);
  } else {
    assert(validSerieMethod()(m_store, m_selectedSeries));
    dataView()->setDisplayBanner(true);
    dataView()->selectViewForSeries(m_selectedSeries);
    highlightSelection();
  }
}

void DataViewController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == m_tabController) {
    assert(m_tabController != nullptr);
    if (header()->selectedButton() >= 0) {
      header()->setSelectedButton(-1);
    } else if (m_store->hasValidSeries(validSerieMethod())) {
      assert(m_selectedSeries >= 0);
      dataView()->deselectViewForSeries(m_selectedSeries);
      dataView()->setDisplayBanner(false);
    }
  }
}

void DataViewController::sanitizeSeriesIndex() {
  // Sanitize m_selectedSeries
  if (m_selectedSeries < 0 || !validSerieMethod()(m_store, m_selectedSeries)) {
    for (int series = 0; series < Store::k_numberOfSeries; series++) {
      if (validSerieMethod()(m_store, series)) {
        m_selectedSeries = series;
        return;
      }
    }
    assert(false);
  }
}

bool DataViewController::moveSelectionVertically(int direction) {
  int nextSelectedSubview = nextSubviewWhenMovingVertically(direction);
  if (nextSelectedSubview >= m_store->numberOfValidSeries(validSerieMethod())) {
    return false;
  }
  dataView()->deselectViewForSeries(m_selectedSeries);
  if (nextSelectedSubview < 0) {
    dataView()->setDisplayBanner(false);
    header()->setSelectedButton(0);
  } else {
    int previousSelectedSeries = m_selectedSeries;
    m_selectedSeries = m_store->indexOfKthValidSeries(nextSelectedSubview, validSerieMethod());
    updateHorizontalIndexAfterSelectingNewSeries(previousSelectedSeries);
    dataView()->selectViewForSeries(m_selectedSeries);
    highlightSelection();
  }
  return true;
}

}
