#include "statistic_graph_controller.h"

#include <escher/clipboard.h>

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

StatisticGraphController::StatisticGraphController(StackViewController * stack,
                                                   Statistic * statistic) :
      Page(stack), m_graphView(statistic, &m_range), m_statistic(statistic) {
}

ViewController::TitlesDisplay StatisticGraphController::titlesDisplay() {
  bool isCategoricalGraph = App::app()->subapp() == Data::SubApp::Tests &&
                            App::app()->test() == Data::Test::Categorical;
  return isCategoricalGraph ? ViewController::TitlesDisplay::DisplayLastTwoTitles
                            : ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char * StatisticGraphController::title() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    char zBuffer[Constants::k_shortBufferSize];
    char pBuffer[Constants::k_shortBufferSize];
    defaultConvertFloatToText(m_statistic->testCriticalValue(), zBuffer, sizeof(zBuffer));
    defaultConvertFloatToText(m_statistic->pValue(), pBuffer, sizeof(pBuffer));
    const char * format = I18n::translate(I18n::Message::StatisticGraphControllerTestTitleFormat);
    snprintf(m_titleBuffer, sizeof(m_titleBuffer), format, zBuffer, pBuffer);
  } else {
    const char * format = I18n::translate(
        I18n::Message::StatisticGraphControllerIntervalTitleFormat);
    char marginOfErrorBuffer[Constants::k_shortBufferSize];
    defaultConvertFloatToText(m_statistic->marginOfError(),
                              marginOfErrorBuffer,
                              sizeof(marginOfErrorBuffer));
    snprintf(m_titleBuffer, sizeof(m_titleBuffer), format, marginOfErrorBuffer);
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
    char lowerBound[Constants::k_shortBufferSize];
    char upperBound[Constants::k_shortBufferSize];
    defaultConvertFloatToText(m_statistic->estimate() - m_statistic->marginOfError(),
                              lowerBound,
                              sizeof(lowerBound));
    defaultConvertFloatToText(m_statistic->estimate() + m_statistic->marginOfError(),
                              upperBound,
                              sizeof(upperBound));
    snprintf(copyBuffer, sizeof(copyBuffer), "[[%s,%s]]", lowerBound, upperBound);
    Escher::Clipboard::sharedClipboard()->store(copyBuffer, strlen(copyBuffer));
    return true;
  }
  return false;
}

}  // namespace Probability
