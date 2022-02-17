#ifndef PROBABILITY_CONTROLLERS_INTERVAL_GRAPH_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_INTERVAL_GRAPH_CONTROLLER_H

#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "probability/gui/interval_conclusion_view.h"
#include "probability/gui/interval_curve_view.h"
#include "probability/gui/statistic_graph_view.h"
#include "probability/constants.h"

namespace Probability {

class IntervalGraphController : public Escher::ViewController {
public:
  IntervalGraphController(Escher::StackViewController * stack, Interval * interval);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  constexpr static int k_titleBufferSize = sizeof("ME=") + Constants::k_shortFloatNumberOfChars;
  char m_titleBuffer[k_titleBufferSize];
  IntervalCurveView m_curveView;
  IntervalConclusionView m_conclusionView;
  StatisticGraphView m_graphView;
  Interval * m_interval;
};

}  // namespace Probability
#endif /* PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H */
