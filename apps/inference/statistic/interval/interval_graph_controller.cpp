#include "interval_graph_controller.h"
#include <escher/clipboard.h>
#include <poincare/print.h>
#include <inference/app.h>

namespace Inference {

IntervalGraphController::IntervalGraphController(Escher::StackViewController * stack, Interval * interval) :
      Escher::ViewController(stack),
      m_intervalBuffer(),
      m_graphView(interval, &m_selectedIntervalIndex),
      m_originalInterval(interval) {
}

Escher::ViewController::TitlesDisplay IntervalGraphController::titlesDisplay() {
  return ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char * IntervalGraphController::title() {
  interval()->setGraphTitle(m_titleBuffer, sizeof(m_titleBuffer));
  return m_titleBuffer;
}

void IntervalGraphController::viewWillAppear() {
  m_originalInterval->computeCurveViewRange();
  // Copy the interval into a local buffer
  memcpy(static_cast<void*>(&m_intervalBuffer), static_cast<void*>(m_originalInterval), sizeof(IntervalBuffer));
  resetSelectedInterval();
}

bool IntervalGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto || event == Ion::Events::Var) {
    // Copy confidence interval as matrix
    char copyBuffer[2 * Constants::k_shortBufferSize + 4];
    Poincare::Print::CustomPrintf(copyBuffer, sizeof(copyBuffer), "[[%*.*ed,%*.*ed]]",
        interval()->estimate() - interval()->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        interval()->estimate() + interval()->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
    if (event == Ion::Events::Sto || event == Ion::Events::Var) {
      App::app()->storeValue(copyBuffer);
    } else {
      Escher::Clipboard::SharedClipboard()->store(copyBuffer);
    }
    return true;
  }
  if ((event == Ion::Events::Up && m_selectedIntervalIndex > 0) || (event == Ion::Events::Down && m_selectedIntervalIndex + 1 < Interval::k_numberOfDisplayedIntervals)) {
    selectAdjacentInterval(event == Ion::Events::Up);
    static_cast<Escher::StackViewController*>(parentResponder())->view()->reload();
    return true;
  }
  return false;
}

void IntervalGraphController::selectAdjacentInterval(bool goUp) {
  m_selectedIntervalIndex += goUp ? -1 : 1;
  interval()->setThreshold(Interval::DisplayedIntervalThresholdAtIndex(m_originalInterval->threshold(), m_selectedIntervalIndex));
  interval()->compute();
  intervalDidChange();
}

void IntervalGraphController::resetSelectedInterval() {
  m_selectedIntervalIndex = Interval::MainDisplayedIntervalThresholdIndex(m_originalInterval->threshold());
  intervalDidChange();
}

void IntervalGraphController::intervalDidChange() {
  m_graphView.conclusionView()->setInterval(interval()->estimate(), interval()->marginOfError());
  m_graphView.reload();
}

}  // namespace Inference