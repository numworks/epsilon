#pragma once

#include <escher/buffer_text_view.h>
#include <escher/button_row_controller.h>
#include <escher/tab_view_controller.h>

#include "../../shared/graph_button_row_delegate.h"
#include "../data/store.h"
#include "pie_banner_view.h"
#include "pie_displayed_data_controller.h"
#include "pie_graph_view.h"

namespace Statistics::Categorical {

/* Main controller of the pie graph page.
 * Handles events and contains the 3 subviews (title, pie graph and banner). */
class PieGraphController : public Escher::ViewController,
                           public GraphButtonRowDelegate {
 public:
  PieGraphController(Escher::Responder* parentResponder,
                     Escher::ButtonRowController* header,
                     Escher::TabViewController* tabController,
                     Escher::StackViewController* stackViewController,
                     Escher::ViewController* typeViewController, Store* store);

  void initView() override;
  void viewWillAppear() override;
  Escher::View* view() override { return &m_view; }

  // ButtonRowDelegate
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override;
  Escher::ButtonCell* buttonAtIndex(
      int index, Escher::ButtonRowController::Position position) const override;

  void handleResponderChainEvent(Responder::ResponderChainEvent event) override;
  bool handleEvent(Ion::Events::Event event) override;

  void pushDataSelectionMenu() {
    stackController()->push(&m_displayedDataController);
  }

 private:
  class ContentView : public Escher::View {
   public:
    ContentView(Store* store);

    void setGroup(int group, Store* store);
    void toggleSelection(bool isSelected);
    void nextActiveCategory(int direction);

   private:
    void reload() { m_pieGraphView.reload(); }

    int numberOfSubviews() const override { return 3; }
    void layoutSubviews(bool force) override;
    View* subviewAtIndex(int index) override;

    Escher::BufferTextView<sizeof(Store::Label)> m_groupTitleView;
    PieGraphView m_pieGraphView;
    PieBannerView m_sideBannerView;
  };

  ContentView m_view;
  Escher::SimpleButtonCell m_displayedDataButton;
  DisplayedDataController m_displayedDataController;
  Store* m_store;
  Escher::TabViewController* m_tabController;
  bool m_isPieGraphSelected;
};

}  // namespace Statistics::Categorical
