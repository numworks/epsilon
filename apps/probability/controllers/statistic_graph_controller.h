#ifndef APPS_PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H

#include <escher/buffer_table_cell.h>
#include <escher/stack_view_controller.h>

#include "probability/gui/statistic_graph_view.h"
#include "probability/gui/selectable_cell_list_controller.h"

namespace Probability {

class StatisticGraphController : public Page, public LegendPositionDataSource {
public:
  StatisticGraphController(StackViewController * stack, Statistic * statistic);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool shouldPositionLegendLeft() override;

private:
  constexpr static int k_titleBufferSize = 30;
  char m_titleBuffer[k_titleBufferSize];
  StatisticGraphView m_graphView;
  Statistic * m_statistic;
};
}  // namespace Probability
#endif /* APPS_PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H */
