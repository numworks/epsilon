#ifndef INFERENCE_STATISTIC_INTERVAL_INTERVAL_GRAPH_CONTROLLER_H
#define INFERENCE_STATISTIC_INTERVAL_INTERVAL_GRAPH_CONTROLLER_H

#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "inference/statistic/interval/interval_graph_view.h"
#include "inference/constants.h"

namespace Inference {

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
  IntervalGraphView m_graphView;
  Interval * m_interval;
};

}  // namespace Inference
#endif /* INFERENCE_STATISTIC_INTERVAL_INTERVAL_GRAPH_CONTROLLER_H */
