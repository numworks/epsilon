#include "graph_type_controller.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Statistics;

GraphTypeController::GraphTypeController(Escher::Responder * parentResponder,
                               Escher::TabViewController * tabController,
                               Escher::StackViewController * stackView,
                               Store * store,
                               GraphViewModel * graphViewModel) :
    Escher::SelectableCellListPage<Escher::TransparentImageCellWithMessage, k_numberOfCells, Escher::RegularListViewDataSource>(parentResponder),
    m_tabController(tabController),
    m_stackViewController(stackView),
    m_store(store),
    m_graphViewModel(graphViewModel) {
  selectRow(GraphViewModel::IndexOfGraphView(m_graphViewModel->selectedGraphView()));
  for (uint8_t i = 0; i < GraphViewModel::k_numberOfGraphViews; i++) {
    GraphViewModel::GraphView graphView = GraphViewModel::GraphViewAtIndex(i);
    cellAtIndex(i)->setMessage(GraphViewModel::MessageForGraphView(graphView));
    cellAtIndex(i)->setImage(GraphViewModel::ImageForGraphView(graphView));
  }
}

void GraphTypeController::didBecomeFirstResponder() {
  selectRow(GraphViewModel::IndexOfGraphView(m_graphViewModel->selectedGraphView()));
  m_selectableTableView.reloadData(true);
}

bool GraphTypeController::handleEvent(Ion::Events::Event & event) {
  if ((event == Ion::Events::Up && selectedRow() == 0) || (event == Ion::Events::Back && m_store->graphViewHasBeenInvalidated())) {
    /* If m_store->graphViewHasBeenInvalidated(), there isn't a previously
     * selected graph view, so Back selects the tab instead. */
    m_tabController->selectTab();
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_store->graphViewHasBeenSelected();
    m_graphViewModel->selectGraphView(GraphViewModel::GraphViewAtIndex(selectedRow()));
    m_stackViewController->pop();
    return true;
  }
  return false;
}
