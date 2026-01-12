#pragma once

#include <apps/i18n.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <escher/tab_view_controller.h>

#include "../../shared/graph_button_row_delegate.h"
#include "../data/store.h"
#include "bar_banner_view.h"
#include "bar_graph_view.h"

namespace Statistics::Categorical {

/* Main controller of the bar graph page.
 * Handles events and contains the 2 subviews (bar graph and banner). */
class BarGraphController : public Escher::ViewController,
                           public GraphButtonRowDelegate {
 public:
  BarGraphController(Escher::Responder* parentResponder,
                     Escher::ButtonRowController* header,
                     Escher::TabViewController* tabController,
                     Escher::StackViewController* stackViewController,
                     Escher::ViewController* typeViewController, Store* store)
      : Escher::ViewController(parentResponder),
        GraphButtonRowDelegate(header, stackViewController, this,
                               typeViewController),
        m_view(store),
        m_tabController(tabController) {}

  void initView() override;
  void viewWillAppear() override;
  Escher::View* view() override { return &m_view; }
  void handleResponderChainEvent(Responder::ResponderChainEvent event) override;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  class ContentView : public Escher::View {
    friend class BarGraphController;

   public:
    ContentView(Store* store);

    void scrollVertical(bool down) {
      m_barGraphView.scrollToContentPoint(KDPoint(
          m_barGraphView.contentOffset().x(), down ? bounds().height() : 0));
    }
    bool isScrolledDown() const {
      return m_barGraphView.contentOffset().y() > 0;
    }
    void toggleSelection(bool isSelected) {
      m_isBarGraphSelected = isSelected;
      m_barGraphView.toggleSelection(isSelected);
      m_bannerView.toggleSelection(isSelected);
    }
    bool isBarGraphSelected() const { return m_isBarGraphSelected; }
    /* Select the next active category and group in direction, or return with
     * the current selection if there is none. */
    void selectNextActiveValue(int direction);
    void updateBannerView();

   private:
    int numberOfSubviews() const override { return 2; }
    void layoutSubviews(bool force = false) override;
    View* subviewAtIndex(int index) override;

    Store* m_store;
    BarGraphView m_barGraphView;
    BarBannerView m_bannerView;
    bool m_isBarGraphSelected;
  };

  ContentView m_view;
  Escher::TabViewController* m_tabController;
};

}  // namespace Statistics::Categorical
