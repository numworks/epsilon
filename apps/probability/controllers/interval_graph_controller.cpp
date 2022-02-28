#include "interval_graph_controller.h"
#include <escher/clipboard.h>
#include <poincare/print.h>

namespace Probability {

IntervalGraphController::IntervalGraphController(Escher::StackViewController * stack, Interval * interval) :
      Escher::ViewController(stack),
      m_curveView(interval),
      m_graphView(&m_curveView, &m_conclusionView),
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
  m_conclusionView.setInterval(m_interval->estimate(), m_interval->marginOfError());
  m_interval->computeCurveViewRange();
  m_graphView.reload();
}

bool IntervalGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy) {
    // Copy confidence interval as matrix
    char copyBuffer[2 * Constants::k_shortBufferSize + 4];
    Poincare::Print::customPrintf(copyBuffer, sizeof(copyBuffer), "[[%*.*ed,%*.*ed]]",
        m_interval->estimate() - m_interval->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        m_interval->estimate() + m_interval->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
    Escher::Clipboard::sharedClipboard()->store(copyBuffer, strlen(copyBuffer));
    return true;
  }
  return false;
}

}  // namespace Probability
