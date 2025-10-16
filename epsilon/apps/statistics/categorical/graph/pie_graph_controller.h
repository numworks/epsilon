#pragma once

#include <apps/i18n.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <escher/tab_view_controller.h>

#include "../../graph/graph_button_row_delegate.h"
#include "../data/store.h"
#include "pie_displayed_data_controller.h"

namespace Statistics::Categorical {

class PieGraphController : public Escher::AlternateEmptyViewController,
                           public Escher::AlternateEmptyViewDelegate,
                           public GraphButtonRowDelegate {
 public:
  PieGraphController(Escher::Responder* parentResponder,
                     Escher::ButtonRowController* header,
                     Escher::TabViewController* tabController,
                     Escher::StackViewController* stackViewController,
                     Escher::ViewController* typeViewController, Store* store)
      : Escher::AlternateEmptyViewController(parentResponder, this, this),
        GraphButtonRowDelegate(header, stackViewController, this,
                               typeViewController),
        m_displayedDataButton(
            this, I18n::Message::DisplayedData,
            Escher::Invocation::Builder<PieGraphController>(
                [](PieGraphController* delegate, void* sender) {
                  delegate->pushDataSelectionMenu();
                  return true;
                },
                this),
            KDFont::Size::Small),
        m_displayedDataController(this, tabController, stackViewController,
                                  store),
        m_store(store),
        m_tabController(tabController) {}

  void initView() override {
    if (!m_store->isGroupActive(m_store->getSelectedGroupForPieGraph())) {
      for (uint8_t group = 0; group < Store::k_maxNumberOfGroups; group++) {
        if (m_store->isGroupActive(group)) {
          m_store->setSelectedGroupForPieGraph(group);
          return;
        }
      }
      OMG::unreachable();
    }
  }

  // ButtonRowDelegate
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override {
    assert(position == Escher::ButtonRowController::Position::Top);
    bool displayedDataButton = m_store->numberOfActiveGroups() >= 2;
    return displayedDataButton +
           GraphButtonRowDelegate::numberOfButtons(position);
  }
  Escher::ButtonCell* buttonAtIndex(
      int index,
      Escher::ButtonRowController::Position position) const override {
    assert(position == Escher::ButtonRowController::Position::Top);
    if (index == 1) {
      return const_cast<Escher::SimpleButtonCell*>(&m_displayedDataButton);
    }
    return GraphButtonRowDelegate::buttonAtIndex(index, position);
  }

  void handleResponderChainEvent(
      Responder::ResponderChainEvent event) override {
    if (event.type == ResponderChainEventType::HasBecomeFirst) {
      header()->setSelectedButton(0);
    }
  }

  bool handleEvent(Ion::Events::Event event) override {
    int selectedButton = header()->selectedButton();
    if (selectedButton >= 0) {
      if (event == Ion::Events::Up || event == Ion::Events::Back) {
        header()->setSelectedButton(-1);
        m_tabController->selectTab();
        return true;
      }
      /*
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
    */
    }
    return false;
  }

  // TEMP: AlternateEmptyViewDelegate
  bool isEmpty() const override { return true; }
  I18n::Message emptyMessage() override { return I18n::Message::PieGraph; }
  Escher::Responder* responderWhenEmpty() override { return this; }
  void pushDataSelectionMenu() {
    stackController()->push(&m_displayedDataController);
  }

 private:
  Escher::SimpleButtonCell m_displayedDataButton;
  DisplayedDataController m_displayedDataController;
  Store* m_store;
  Escher::TabViewController* m_tabController;
};

}  // namespace Statistics::Categorical
