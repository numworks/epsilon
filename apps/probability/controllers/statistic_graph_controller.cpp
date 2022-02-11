#include "statistic_graph_controller.h"

#include <escher/clipboard.h>
#include <poincare/print.h>

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

StatisticGraphController::StatisticGraphController(StackViewController * stack,
                                                   Statistic * statistic) :
      Escher::ViewController(stack), m_graphView(statistic, &m_range), m_statistic(statistic) {
}

ViewController::TitlesDisplay StatisticGraphController::titlesDisplay() {
  if (App::app()->subapp() == Data::SubApp::Intervals || App::app()->categoricalType() == Data::CategoricalType::Goodness) {
    return ViewController::TitlesDisplay::DisplayLastThreeTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastFourTitles;
}

const char * StatisticGraphController::title() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    const char * format = I18n::translate(graphTitleFormatForTest(App::app()->test(), App::app()->testType()));
    Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), format,
        m_statistic->testCriticalValue(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        m_statistic->pValue(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  } else {
    const char * format = I18n::translate(I18n::Message::StatisticGraphControllerIntervalTitleFormat);
    Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), format,
        m_statistic->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  }
  return m_titleBuffer;
}

void StatisticGraphController::didBecomeFirstResponder() {
  App::app()->setPage(Data::Page::Graph);
  m_graphView.setStatistic(m_statistic);
  m_range.setStatistic(m_statistic);
  if (App::app()->subapp() == Data::SubApp::Intervals) {
    m_graphView.intervalConclusionView()->setInterval(m_statistic->estimate(),
                                                      m_statistic->marginOfError());
  }
  m_graphView.reload();
}

bool StatisticGraphController::handleEvent(Ion::Events::Event event) {
  if (App::app()->subapp() == Data::SubApp::Intervals && event == Ion::Events::Copy) {
    // Copy confidence interval as matrix
    char copyBuffer[2 * Constants::k_shortBufferSize + 4];
    Poincare::Print::customPrintf(copyBuffer, sizeof(copyBuffer), "[[%*.*ed,%*.*ed]]",
        m_statistic->estimate() - m_statistic->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        m_statistic->estimate() + m_statistic->marginOfError(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
    Escher::Clipboard::sharedClipboard()->store(copyBuffer, strlen(copyBuffer));
    return true;
  }
  return false;
}

}  // namespace Probability
