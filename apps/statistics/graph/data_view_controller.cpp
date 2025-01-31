#include "data_view_controller.h"

#include <assert.h>
#include <escher/container.h>

#include "../app.h"

namespace Statistics {

DataViewController::DataViewController(
    Escher::Responder* parentResponder,
    Escher::TabViewController* tabController,
    Escher::ButtonRowController* header,
    Escher::StackViewController* stackViewController,
    Escher::ViewController* typeViewController, Store* store)
    : ViewController(parentResponder),
      GraphButtonRowDelegate(header, stackViewController, this,
                             typeViewController),
      m_store(store),
      m_tabController(tabController) {
  assert(numberOfButtons(Escher::ButtonRowController::Position::Top) > 0);
}

int DataViewController::selectedSeries() const {
  return App::app()->snapshot()->selectedSeries();
}

void DataViewController::setSelectedSeries(int selectedSeries) {
  assert(selectedSeries >= INT8_MIN && selectedSeries <= INT8_MAX);
  App::app()->snapshot()->setSelectedSeries(
      static_cast<int8_t>(selectedSeries));
}

int DataViewController::selectedIndex() const {
  return App::app()->snapshot()->selectedIndex();
}

void DataViewController::setSelectedIndex(int selectedIndex) {
  assert(selectedIndex >= INT16_MIN && selectedIndex <= INT16_MAX);
  App::app()->snapshot()->setSelectedIndex(static_cast<int16_t>(selectedIndex));
}

void DataViewController::viewWillAppear() {
  ViewController::viewWillAppear();
  sanitizeSeriesIndex();
  dataView()->setDisplayBanner(true);
  dataView()->selectViewForSeries(selectedSeries());
  highlightSelection();

  /* Call children's viewWillAppear implementation after sanitizing selected
   * series index and before reloading banner and the dataView */
  viewWillAppearBeforeReload();

  header()->setSelectedButton(-1);
  reloadBannerView();
  dataView()->reload();
}

bool DataViewController::handleEvent(Ion::Events::Event event) {
  int selectedButton = header()->selectedButton();
  if (selectedButton >= 0) {
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::Down &&
        m_store->hasActiveSeries(activeSeriesMethod())) {
      header()->setSelectedButton(-1);
      dataView()->setDisplayBanner(true);
      dataView()->selectViewForSeries(selectedSeries());
      highlightSelection();
      reloadBannerView();
      Escher::App::app()->setFirstResponder(this);
      return true;
    }
    return buttonAtIndex(selectedButton,
                         Escher::ButtonRowController::Position::Top)
        ->handleEvent(event);
  }
  assert(selectedSeries() >= 0 &&
         m_store->hasActiveSeries(activeSeriesMethod()));
  bool isVerticalEvent =
      (event == Ion::Events::Down || event == Ion::Events::Up);
  if ((isVerticalEvent || event == Ion::Events::Left ||
       event == Ion::Events::Right)) {
    if (isVerticalEvent ? moveSelectionVertically(event.direction())
                        : moveSelectionHorizontally(event.direction())) {
      if (reloadBannerView()) {
        dataView()->reload();
      }
      return true;
    }
  }
  return false;
}

void DataViewController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::DidEnter) {
    if (!m_store->hasActiveSeries(activeSeriesMethod()) ||
        !dataView()->plotViewForSeries(selectedSeries())->hasFocus()) {
      header()->setSelectedButton(0);
    } else {
      assert(activeSeriesMethod()(m_store, selectedSeries()));
      dataView()->setDisplayBanner(true);
      dataView()->selectViewForSeries(selectedSeries());
      highlightSelection();
    }
  } else if (event.type == ResponderChainEventType::WillExit) {
    if (event.nextFirstResponder == m_tabController) {
      assert(m_tabController != nullptr);
      if (header()->selectedButton() >= 0) {
        header()->setSelectedButton(-1);
      } else if (m_store->hasActiveSeries(activeSeriesMethod())) {
        assert(selectedSeries() >= 0);
        dataView()->deselectViewForSeries(selectedSeries());
        dataView()->setDisplayBanner(false);
      }
    }
  } else {
    ViewController::handleResponderChainEvent(event);
  }
}

void DataViewController::sanitizeSeriesIndex() {
  // Sanitize selectedSeries()
  if (selectedSeries() < 0 ||
      !activeSeriesMethod()(m_store, selectedSeries())) {
    for (int8_t series = 0; series < Store::k_numberOfSeries; series++) {
      if (activeSeriesMethod()(m_store, series)) {
        setSelectedSeries(series);
        return;
      }
    }
    assert(false);
  }
}

bool DataViewController::moveSelectionVertically(
    OMG::VerticalDirection direction) {
  int nextSelectedSubview = nextSubviewWhenMovingVertically(direction);
  if (nextSelectedSubview >=
      m_store->numberOfActiveSeries(activeSeriesMethod())) {
    return false;
  }
  dataView()->deselectViewForSeries(selectedSeries());
  if (nextSelectedSubview < 0) {
    dataView()->setDisplayBanner(false);
    header()->setSelectedButton(0);
  } else {
    int previousSelectedSeries = selectedSeries();
    setSelectedSeries(m_store->seriesIndexFromActiveSeriesIndex(
        nextSelectedSubview, activeSeriesMethod()));
    updateHorizontalIndexAfterSelectingNewSeries(previousSelectedSeries);
    dataView()->selectViewForSeries(selectedSeries());
    highlightSelection();
  }
  return true;
}

}  // namespace Statistics
