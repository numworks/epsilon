#ifndef APPS_PROBABILITY_CONTROLLERS_GRAPH_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_GRAPH_CONTROLLER_H

#include <escher/buffer_table_cell.h>
#include <escher/stack_view_controller.h>

#include "probability/gui/selectable_cell_list_controller.h"
#include "probability/gui/distribution_graph_view.h"

namespace Probability {

class GraphController : public Page {
 public:
  GraphController(StackViewController * stack) : Page(stack) {}
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override { m_graphView.computeMode(); }

private:
  GraphView m_graphView;
};
}  // namespace Probability
#endif /* APPS_PROBABILITY_CONTROLLERS_GRAPH_CONTROLLER_H */
