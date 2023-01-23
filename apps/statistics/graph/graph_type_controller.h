#ifndef STATISTICS_GRAPH_TYPE_CONTROLLER_H
#define STATISTICS_GRAPH_TYPE_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/transparent_image_cell_with_message.h>
#include <escher/alternate_empty_view_delegate.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>
#include <apps/i18n.h>
#include "graph_view_model.h"
#include "../store.h"

namespace Statistics {

constexpr int k_numberOfCells = 4;
class GraphTypeController : public Escher::AlternateEmptyViewDelegate, public Escher::SelectableCellListPage<Escher::TransparentImageCellWithMessage, k_numberOfCells, Escher::RegularListViewDataSource> {
public:
  GraphTypeController(Escher::Responder * parentResponder,
                 Escher::TabViewController * tabController,
                 Escher::StackViewController * stackView,
                 Store * store,
                 GraphViewModel * graphViewModel);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return !m_store->hasActiveSeries(); }
  I18n::Message emptyMessage() override { return I18n::Message::NoDataToPlot; }
  Escher::Responder * responderWhenEmpty() override;

  // SelectableCellListPage
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  const char * title() override { return I18n::translate(I18n::Message::StatisticsGraphType); }

private:
  Escher::TabViewController * m_tabController;
  Escher::StackViewController * m_stackViewController;
  Store * m_store;
  GraphViewModel * m_graphViewModel;
};

}

#endif
