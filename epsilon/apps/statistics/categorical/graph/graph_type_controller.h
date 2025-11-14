#pragma once

#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/alternate_view_controller.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <escher/transparent_image_view.h>

#include "graph_view_model.h"

namespace Statistics::Categorical {

using GraphTypeCell =
    Escher::MenuCell<Escher::TransparentImageView, Escher::MessageTextView>;

class GraphTypeController
    : public Escher::AlternateViewDelegate,
      public Escher::UniformSelectableListController<GraphTypeCell, 2> {
 public:
  GraphTypeController(Escher::Responder* parentResponder,
                      Escher::TabViewController* tabViewController,
                      Escher::StackViewController* stackView,
                      GraphViewModel* graphViewModel)
      : Escher::UniformSelectableListController<GraphTypeCell, k_numberOfCells>(
            parentResponder),
        m_tabController(tabViewController),
        m_stackViewController(stackView),
        m_graphViewModel(graphViewModel),
        m_viewHasBeenSelected(false) {
    selectRow(GraphViewModel::IndexOfGraphView(
        m_graphViewModel->selectedGraphView()));
    for (uint8_t i = 0; i < GraphViewModel::k_numberOfGraphViews; i++) {
      GraphViewModel::GraphView graphView = GraphViewModel::GraphViewAtIndex(i);
      cell(i)->subLabel()->setGlyphFormat(
          Escher::GlyphsView::FormatForWidgetType(
              Escher::CellWidget::Type::Label));
      cell(i)->subLabel()->setMessage(
          GraphViewModel::MessageForGraphView(graphView));
      cell(i)->label()->setImage(GraphViewModel::ImageForGraphView(graphView));
    }
  };

  // UniformSelectableListController
  bool handleEvent(Ion::Events::Event event) override {
    if ((event == Ion::Events::Up && selectedRow() == 0) ||
        (event == Ion::Events::Back && !m_viewHasBeenSelected)) {
      /* If no view has been selected then there is no previous graph view to go
       * back to, so Back selects the tab instead. */
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::OK || event == Ion::Events::EXE ||
        event == Ion::Events::Right) {
      m_viewHasBeenSelected = true;
      m_graphViewModel->selectGraphView(
          GraphViewModel::GraphViewAtIndex(selectedRow()));
      m_stackViewController->pop();
      return true;
    }
    return false;
  }
  const char* title() const override {
    return I18n::translate(I18n::Message::Type);
  }

  // AlternateViewDelegate (which graphView is shown)
  int activeViewControllerIndex() const override {
    return GraphViewModel::IndexOfGraphView(
        m_graphViewModel->selectedGraphView());
  }
  Escher::ViewController::TitlesDisplay alternateViewTitlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::NeverDisplayOwnTitle;
  }
  void activeViewDidBecomeFirstResponder(
      Escher::ViewController* activeViewController) override {
    if (!m_viewHasBeenSelected) {
      m_stackViewController->push(this);
    } else {
      Escher::App::app()->setFirstResponder(activeViewController);
    }
  }

  Escher::TabViewController* m_tabController;
  Escher::StackViewController* m_stackViewController;
  GraphViewModel* m_graphViewModel;
  bool m_viewHasBeenSelected;
};

}  // namespace Statistics::Categorical
