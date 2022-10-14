#include "interval_graph_controller.h"
#include <escher/clipboard.h>
#include <poincare/print.h>
#include <inference/app.h>

namespace Inference {

IntervalGraphController::IntervalGraphController(Escher::StackViewController * stack, Interval * interval) :
      Escher::ViewController(stack),
      m_graphView(interval),
      m_interval(interval) {
}

Escher::ViewController::TitlesDisplay IntervalGraphController::titlesDisplay() {
  return ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char * IntervalGraphController::title() {
  m_interval->setGraphTitle(m_titleBuffer, sizeof(m_titleBuffer));
  return m_titleBuffer;
}

void IntervalGraphController::didBecomeFirstResponder() {
  m_interval->computeCurveViewRange();
  m_graphView.reload(true);
}

bool IntervalGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto || event == Ion::Events::Var) {
    // Copy confidence interval as matrix
    char copyBuffer[2 * Constants::k_shortBufferSize + 4];
    Poincare::Print::CustomPrintf(copyBuffer, sizeof(copyBuffer), "[[%*.*ed,%*.*ed]]",
        m_interval->estimate() - m_interval->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        m_interval->estimate() + m_interval->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
    if (event == Ion::Events::Sto || event == Ion::Events::Var) {
      App::app()->storeValue(copyBuffer);
    } else {
      Escher::Clipboard::sharedClipboard()->store(copyBuffer);
    }
    return true;
  }
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    m_graphView.selectAdjacentInterval(event == Ion::Events::Up);
    return true;
  }
  return false;
}

}  // namespace Inference
