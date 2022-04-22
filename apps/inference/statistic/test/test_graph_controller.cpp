#include "test_graph_controller.h"
#include <poincare/print.h>

namespace Inference {

TestGraphController::TestGraphController(Escher::StackViewController * stack, Test * test) :
      Escher::ViewController(stack),
      m_graphView(test),
      m_test(test) {
}

Escher::ViewController::TitlesDisplay TestGraphController::titlesDisplay() {
  // TODO: improve StackViewController way of picking Stack titles to display
  if (m_test->significanceTestType() == SignificanceTestType::Categorical && m_test->categoricalType() == CategoricalType::GoodnessOfFit) {
    return ViewController::TitlesDisplay::DisplayLastAndThirdToLast;
  }
  return ViewController::TitlesDisplay::DisplayLastFourTitles;
}

const char * TestGraphController::title() {
  m_test->setGraphTitle(m_titleBuffer, sizeof(m_titleBuffer));
  return m_titleBuffer;
}

void TestGraphController::didBecomeFirstResponder() {
  m_test->computeCurveViewRange();
  m_graphView.reload();
}

}  // namespace Inference
