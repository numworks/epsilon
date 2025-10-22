#pragma once

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/button_row_controller.h>
#include <escher/tab_view_controller.h>

#include "../../graph/graph_button_row_delegate.h"
#include "../data/store.h"
#include "pie_displayed_data_controller.h"
#include "pie_graph_view.h"

namespace Statistics::Categorical {

class PieGraphController : public Escher::ViewController,
                           public GraphButtonRowDelegate {
 public:
  PieGraphController(Escher::Responder* parentResponder,
                     Escher::ButtonRowController* header,
                     Escher::TabViewController* tabController,
                     Escher::StackViewController* stackViewController,
                     Escher::ViewController* typeViewController, Store* store)
      : Escher::ViewController(parentResponder),
        GraphButtonRowDelegate(header, stackViewController, this,
                               typeViewController),
        m_view(store),
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
        m_tabController(tabController),
        m_isPieGraphSelected(true) {}

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

  void viewWillAppear() override {
    assert(m_store->isGroupActive(m_store->getSelectedGroupForPieGraph()));
    int group = m_store->getSelectedGroupForPieGraph();
    char buffer[sizeof(Store::Label)];
    m_store->getGroupName(group, buffer, sizeof(buffer));
    m_view.setTitleText(buffer);
    m_view.m_pieGraphView.setGroup(group);
    m_view.m_pieGraphView.toggleSelection(true);
    m_view.m_pieGraphView.reload();
    m_isPieGraphSelected = true;
  }

  Escher::View* view() override { return &m_view; }

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
      if (!m_isPieGraphSelected) {
        header()->setSelectedButton(0);
      }
    }
  }

  bool handleEvent(Ion::Events::Event event) override {
    if (header()->selectedButton() >= 0) {
      if (event == Ion::Events::Up || event == Ion::Events::Back) {
        header()->setSelectedButton(-1);
        m_tabController->selectTab();
        return true;
      }
      if (event == Ion::Events::Down) {
        header()->setSelectedButton(-1);
        m_isPieGraphSelected = true;
        m_view.m_pieGraphView.toggleSelection(m_isPieGraphSelected);
        Escher::App::app()->setFirstResponder(this);
        return true;
      }
      return false;
    }
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      m_isPieGraphSelected = false;
      m_view.m_pieGraphView.toggleSelection(m_isPieGraphSelected);
      if (event == Ion::Events::Up) {
        header()->setSelectedButton(0);
      } else {
        m_tabController->selectTab();
      }
      return true;
    }
    if (event == Ion::Events::Left || event == Ion::Events::Right) {
      m_view.m_pieGraphView.nextCategory(event == Ion::Events::Right ? 1 : -1);
      m_view.m_pieGraphView.reload();
      return true;
    }
    return false;
  }

  void pushDataSelectionMenu() {
    stackController()->push(&m_displayedDataController);
  }

 private:
  class ContentView : public Escher::View {
    Escher::BufferTextView<sizeof(Store::Label)> m_groupTitleView;

   public:
    PieGraphView m_pieGraphView;              // TEMP placeholder
    Escher::SolidColorView m_sideBannerView;  // TEMP placeholder

    ContentView(Store* store)
        : m_groupTitleView({.style = {.font = KDFont::Size::Small},
                            .horizontalAlignment = KDGlyph::k_alignCenter}),
          m_pieGraphView(store),
          m_sideBannerView(Escher::Palette::GrayMiddle) {}

    void setTitleText(const char* text) { m_groupTitleView.setText(text); }

   private:
    void layoutSubviews(bool force) override {
      KDRect b = bounds();
      /* Almost like a square, but not quite to make it simple to draw the pie
       * graph. See implementation and comments in [PieGraphView] */
      const int leftSideWidth = b.height() + 5 - b.height() % 5;
      constexpr int k_titleHeight = 25;
      setChildFrame(&m_groupTitleView,
                    KDRect(0, 0, leftSideWidth, k_titleHeight), force);
      setChildFrame(
          &m_pieGraphView,
          KDRect(0, k_titleHeight, leftSideWidth, b.height() - k_titleHeight),
          force);
      setChildFrame(
          &m_sideBannerView,
          KDRect(leftSideWidth, 0, b.width() - leftSideWidth, b.height()),
          force);
    }
    int numberOfSubviews() const override { return 3; }
    View* subviewAtIndex(int index) override {
      switch (index) {
        case 0:
          return &m_groupTitleView;
        case 1:
          return &m_pieGraphView;
        default:
          assert(index == 2);
          return &m_sideBannerView;
      }
    }
  };

  ContentView m_view;
  Escher::SimpleButtonCell m_displayedDataButton;
  DisplayedDataController m_displayedDataController;
  Store* m_store;
  Escher::TabViewController* m_tabController;
  bool m_isPieGraphSelected;
};

}  // namespace Statistics::Categorical
