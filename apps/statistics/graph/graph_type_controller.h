#ifndef STATISTICS_GRAPH_TYPE_CONTROLLER_H
#define STATISTICS_GRAPH_TYPE_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <escher/transparent_image_view.h>

#include "../store.h"
#include "graph_view_model.h"

namespace Statistics {

using GraphTypeCell =
    Escher::MenuCell<Escher::TransparentImageView, Escher::MessageTextView>;

constexpr int k_numberOfCells = 4;
class GraphTypeController
    : public Escher::AlternateEmptyViewDelegate,
      public Escher::SelectableCellListPage<GraphTypeCell, k_numberOfCells,
                                            Escher::RegularListViewDataSource> {
 public:
  GraphTypeController(Escher::Responder* parentResponder,
                      Escher::TabViewController* tabController,
                      Escher::StackViewController* stackView, Store* store,
                      GraphViewModel* graphViewModel);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return !m_store->hasActiveSeries(); }
  I18n::Message emptyMessage() override { return I18n::Message::NoDataToPlot; }
  Escher::Responder* responderWhenEmpty() override;

  // SelectableCellListPage
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  const char* title() override {
    return I18n::translate(I18n::Message::StatisticsGraphType);
  }

 private:
  Escher::TabViewController* m_tabController;
  Escher::StackViewController* m_stackViewController;
  Store* m_store;
  GraphViewModel* m_graphViewModel;
};

}  // namespace Statistics

#endif
