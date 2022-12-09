#include "test_graph_controller.h"
#include <poincare/print.h>

namespace Inference {

TestGraphController::TestGraphController(Escher::StackViewController * stack, Test * test) :
      Escher::ViewController(stack),
      m_graphView(test),
      m_test(test),
      m_zoom(0) {
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
  m_zoom = 0;
  m_zoomSide = true;
  m_test->computeCurveViewRange(0, m_zoomSide);
  m_graphView.setDisplayHint(true);
  m_graphView.reload();
}

bool TestGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus && m_zoom < k_zoomSteps) {
    m_zoom++;
  } else if (event == Ion::Events::Minus && m_zoom > 0) {
    m_zoom--;
  } else if (m_zoom > 0 && m_test->hasTwoSides() && event == Ion::Events::Left && m_zoomSide) {
    m_zoomSide = false;
  } else if (m_zoom > 0 && m_test->hasTwoSides() && event == Ion::Events::Right && !m_zoomSide) {
    m_zoomSide = true;
  } else if (event == Ion::Events::Idle || event == Ion::Events::Back || event == Ion::Events::Home || event == Ion::Events::OnOff) {
    return false;
  } else if (!m_graphView.displayHint() && event != Ion::Events::Shift && event != Ion::Events::Plus && event != Ion::Events::Minus && (!m_test->hasTwoSides() || (event != Ion::Events::Left && event != Ion::Events::Right))) {
      m_graphView.setDisplayHint(true);
      m_graphView.reload();
      return true;
  } else {
    return false;
  }
  m_graphView.setDisplayHint(false);
  m_test->computeCurveViewRange(static_cast<float>(m_zoom) / k_zoomSteps, m_zoomSide);
  m_graphView.reload();
  return true;
}

}  // namespace Inference
