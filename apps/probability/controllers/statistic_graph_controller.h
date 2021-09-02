#ifndef APPS_PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H

#include <escher/stack_view_controller.h>

#include "probability/gui/page_controller.h"
#include "probability/gui/statistic_graph_view.h"
#include "probability/models/statistic_view_range.h"

namespace Probability {

class StatisticGraphController : public Page {
public:
  StatisticGraphController(Escher::StackViewController * stack, Statistic * statistic);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  constexpr static int k_titleBufferSize = sizeof("z= p-value=") +
                                           Constants::k_shortFloatNumberOfChars * 2;
  char m_titleBuffer[k_titleBufferSize];
  StatisticGraphView m_graphView;
  StatisticViewRange m_range;
  Statistic * m_statistic;
};
}  // namespace Probability
#endif /* APPS_PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H */
