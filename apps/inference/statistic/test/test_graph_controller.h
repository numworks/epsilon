#ifndef INFERENCE_STATISTIC_TEST_TEST_GRAPH_CONTROLLER_H
#define INFERENCE_STATISTIC_TEST_TEST_GRAPH_CONTROLLER_H

#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "inference/statistic/test/test_graph_view.h"
#include "inference/constants.h"


namespace Inference {

class TestGraphController : public Escher::ViewController {
public:
  TestGraphController(Escher::StackViewController * stack, Test * test);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  constexpr static int k_titleBufferSize = sizeof("df= α= z= p-value=") + Constants::k_shortFloatNumberOfChars * 4;
  constexpr static int k_zoomSteps = 6;
  char m_titleBuffer[k_titleBufferSize];
  TestGraphView m_graphView;
  Test * m_test;
  /* When the test curve has two interesting sides, we can choose to zoom on
   * the left or the right side. */
  bool m_zoomSide;
  int m_zoom;
};

}  // namespace Inference
#endif /* INFERENCE_STATISTIC_TEST_TEST_GRAPH_CONTROLLER_H */
