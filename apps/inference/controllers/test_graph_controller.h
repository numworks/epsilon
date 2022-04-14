#ifndef PROBABILITY_CONTROLLERS_TEST_GRAPH_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_TEST_GRAPH_CONTROLLER_H

#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "inference/gui/legend_view.h"
#include "inference/gui/statistic_graph_view.h"
#include "inference/gui/test_conclusion_view.h"
#include "inference/gui/test_curve_view.h"
#include "inference/constants.h"


namespace Inference {

class TestGraphController : public Escher::ViewController {
public:
  TestGraphController(Escher::StackViewController * stack, Test * test);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  Escher::View * view() override { return &m_graphView; }
  void didBecomeFirstResponder() override;

private:
  constexpr static int k_titleBufferSize = sizeof("df= α= z= p-value=") + Constants::k_shortFloatNumberOfChars * 4;
  char m_titleBuffer[k_titleBufferSize];
  TestCurveView m_curveView;
  TestConclusionView m_conclusionView;
  LegendView m_legendView;
  StatisticGraphView m_graphView;
  Test * m_test;
};

}  // namespace Inference
#endif /* PROBABILITY_CONTROLLERS_STATISTIC_GRAPH_CONTROLLER_H */
