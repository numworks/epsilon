#include "interval_graph_controller.h"
#include <escher/clipboard.h>
#include <poincare/print.h>
#include <inference/app.h>

namespace Inference {

IntervalGraphController::IntervalGraphController(Escher::StackViewController * stack, Interval * interval) :
      Escher::ViewController(stack),
      m_graphView(interval, &m_selectedIntervalIndex),
      m_interval(interval),
      m_currentEstimate(0.0),
      m_currentMarginOfError(0.0),
      m_currentThreshold(0.0) {
}

Escher::ViewController::TitlesDisplay IntervalGraphController::titlesDisplay() {
  return ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char * IntervalGraphController::title() {
  m_interval->setGraphTitleForValue(m_currentMarginOfError, m_titleBuffer, sizeof(m_titleBuffer));
  return m_titleBuffer;
}

void IntervalGraphController::viewWillAppear() {
  m_interval->computeCurveViewRange();
  resetSelectedInterval();
}

bool IntervalGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut || event == Ion::Events::Sto || event == Ion::Events::Var) {
    // Copy confidence interval as matrix
    char copyBuffer[2 * Constants::k_shortBufferSize + 4];
    Poincare::Print::CustomPrintf(copyBuffer, sizeof(copyBuffer), "[[%*.*ed,%*.*ed]]",
        m_currentEstimate - m_currentMarginOfError, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        m_currentEstimate + m_currentMarginOfError, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
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

void IntervalGraphController::setResultTitleForCurrentValues(char * buffer, size_t bufferSize, bool resultIsTopPage) const {
  m_interval->setResultTitleForValues(m_currentEstimate, m_currentThreshold, buffer, bufferSize, resultIsTopPage);
}

void IntervalGraphController::selectAdjacentInterval(bool goUp) {
  m_selectedIntervalIndex += goUp ? -1 : 1;
  double currentThreshold = m_interval->threshold();
  /* Temporarly change the threshold to compute the values displayed in
   * conclusionView, in titles and in the clipboard */
  m_interval->setThreshold(Interval::DisplayedIntervalThresholdAtIndex(currentThreshold, m_selectedIntervalIndex));
  m_interval->compute();
  saveIntervalValues();
  m_interval->setThreshold(currentThreshold);
  m_interval->compute();
  intervalDidChange();
}

void IntervalGraphController::resetSelectedInterval() {
  m_selectedIntervalIndex = Interval::MainDisplayedIntervalThresholdIndex(m_interval->threshold());
  saveIntervalValues();
  intervalDidChange();
}

void IntervalGraphController::saveIntervalValues() {
  m_currentEstimate = m_interval->estimate();
  m_currentMarginOfError = m_interval->marginOfError();
  m_currentThreshold = m_interval->threshold();
}

void IntervalGraphController::intervalDidChange() {
  m_graphView.conclusionView()->setInterval(m_currentEstimate, m_currentMarginOfError);
  m_graphView.reload();
}

}
