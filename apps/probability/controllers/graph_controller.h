#ifndef APPS_PROBABILITY_CONTROLLERS_GRAPH_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_GRAPH_CONTROLLER_H

#include <escher/buffer_table_cell.h>
#include <escher/stack_view_controller.h>

#include "probability/gui/distribution_graph_view.h"
#include "probability/gui/selectable_cell_list_controller.h"
#include "probability/models/statistic/statistic_view_range.h"

namespace Probability {

class GraphController : public Page {
public:
  GraphController(StackViewController * stack);
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override;

private:
  StatisticViewRange m_rangeLeft;
  StatisticViewRange m_rangeRight;
  GraphView m_graphView;
};
}  // namespace Probability
#endif /* APPS_PROBABILITY_CONTROLLERS_GRAPH_CONTROLLER_H */
