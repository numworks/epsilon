#pragma once

#include <apps/i18n.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <escher/tab_view_controller.h>

#include "../../graph/graph_button_row_delegate.h"

namespace Statistics::Categorical {

class PieGraphController : public Escher::AlternateEmptyViewController,
                           public Escher::AlternateEmptyViewDelegate,
                           public GraphButtonRowDelegate {
 public:
  PieGraphController(Escher::Responder* parentResponder,
                     Escher::ButtonRowController* header,
                     Escher::TabViewController* tabController,
                     Escher::StackViewController* stackViewController,
                     Escher::ViewController* typeViewController)
      : Escher::AlternateEmptyViewController(parentResponder, this, this),
        GraphButtonRowDelegate(header, stackViewController, this,
                               typeViewController),
        m_tabController(tabController) {}

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

 private:
  Escher::TabViewController* m_tabController;
};

}  // namespace Statistics::Categorical
