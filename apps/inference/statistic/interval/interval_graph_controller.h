#ifndef INFERENCE_STATISTIC_INTERVAL_INTERVAL_GRAPH_CONTROLLER_H
#define INFERENCE_STATISTIC_INTERVAL_INTERVAL_GRAPH_CONTROLLER_H

#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "inference/statistic/interval/interval_graph_view.h"
#include "inference/constants.h"
#include "inference/models/statistic_buffer.h"

namespace Inference {

class IntervalGraphController : public Escher::ViewController {
public:
  IntervalGraphController(Escher::StackViewController * stack, Interval * interval);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  Escher::View * view() override { return &m_graphView; }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event & event) override;
  Interval * interval() { return m_intervalBuffer.interval(); }
private:
  constexpr static int k_titleBufferSize = sizeof("ME=") + Constants::k_shortFloatNumberOfChars;
  void resetSelectedInterval();
  void selectAdjacentInterval(bool goUp);
  void intervalDidChange();
  char m_titleBuffer[k_titleBufferSize];
  IntervalBuffer m_intervalBuffer;
  IntervalGraphView m_graphView;
  Interval * m_originalInterval;
  int m_selectedIntervalIndex;
};

}  // namespace Inference
#endif /* INFERENCE_STATISTIC_INTERVAL_INTERVAL_GRAPH_CONTROLLER_H */
