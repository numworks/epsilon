#include "test_graph_controller.h"

#include <poincare/print.h>

#include "inference/controllers/inference_controller.h"
#include "inference/models/significance_test.h"

namespace Inference {

TestGraphController::TestGraphController(Escher::StackViewController* stack,
                                         SignificanceTest* test)
    : InferenceController(test),
      Escher::ViewController(stack),
      m_graphView(test),
      m_zoom(0) {}

Escher::ViewController::TitlesDisplay TestGraphController::titlesDisplay()
    const {
  // TODO: improve StackViewController way of picking Stack titles to display
  if (significanceTestModel()->testType() == TestType::Chi2 &&
      significanceTestModel()->categoricalType() ==
          CategoricalType::GoodnessOfFit) {
    return ViewController::TitlesDisplay::DisplayLastAndThirdToLast;
  }
  if (significanceTestModel()->canChooseDataset()) {
    return ViewController::TitlesDisplay(0b10111);
  }
  return ViewController::TitlesDisplay::DisplayLastFourTitles;
}

const char* TestGraphController::title() const {
  significanceTestModel()->setGraphTitle(m_titleBuffer, sizeof(m_titleBuffer));
  return m_titleBuffer;
}

void TestGraphController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    m_zoom = 0;
    m_zoomSide = true;
    m_mayBeZoomed =
        significanceTestModel()->computeCurveViewRange(0, m_zoomSide);
    m_graphView.setDisplayHint(m_mayBeZoomed);
    m_graphView.reload();
  } else {
    Escher::ViewController::handleResponderChainEvent(event);
  }
}

bool TestGraphController::handleEvent(Ion::Events::Event event) {
  if (!m_mayBeZoomed) {
    return false;
  }
  if (event == Ion::Events::Plus && m_zoom < k_zoomSteps) {
    m_zoom++;
  } else if (event == Ion::Events::Minus && m_zoom > 0) {
    m_zoom--;
  } else if (m_zoom > 0 && significanceTestModel()->hasTwoSides() &&
             event == Ion::Events::Left && m_zoomSide) {
    m_zoomSide = false;
  } else if (m_zoom > 0 && significanceTestModel()->hasTwoSides() &&
             event == Ion::Events::Right && !m_zoomSide) {
    m_zoomSide = true;
  } else if (!event.isKeyPress() || event == Ion::Events::Back ||
             event == Ion::Events::Home || event == Ion::Events::OnOff) {
    return false;
  } else if (!m_graphView.displayHint() && event != Ion::Events::Shift &&
             event != Ion::Events::Plus && event != Ion::Events::Minus &&
             (!significanceTestModel()->hasTwoSides() ||
              (event != Ion::Events::Left && event != Ion::Events::Right))) {
    m_graphView.setDisplayHint(true);
    m_graphView.reload();
    return true;
  } else {
    return false;
  }
  m_graphView.setDisplayHint(false);
  significanceTestModel()->computeCurveViewRange(
      static_cast<float>(m_zoom) / k_zoomSteps, m_zoomSide);
  m_graphView.reload();
  return true;
}

}  // namespace Inference
