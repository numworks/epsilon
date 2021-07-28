#include "statistic_graph_controller.h"

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

StatisticGraphController::StatisticGraphController(StackViewController * stack,
                                                   Statistic * statistic) :
    Page(stack),
    m_graphView(&m_rangeLeft, &m_rangeRight),
    m_rangeLeft(m_graphView.curveViewLeft(), true),
    m_rangeRight(m_graphView.curveViewRight(), false),
    m_statistic(statistic) {
}

ViewController::TitlesDisplay StatisticGraphController::titlesDisplay() {
  bool isCategoricalGraph = App::app()->subapp() == Data::SubApp::Tests &&
                            App::app()->test() == Data::Test::Categorical;
  return isCategoricalGraph ? ViewController::TitlesDisplay::DisplayLastTwoTitles
                            : ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char * StatisticGraphController::title() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    char zBuffer[10];
    char pBuffer[10];
    defaultParseFloat(m_statistic->testCriticalValue(), zBuffer, sizeof(zBuffer));
    defaultParseFloat(m_statistic->pValue(), pBuffer, sizeof(pBuffer));
    snprintf(m_titleBuffer, sizeof(m_titleBuffer), "z=%s p-value=%s", zBuffer, pBuffer);
  } else {
    char MEBuffer[30];
    defaultParseFloat(m_statistic->marginOfError(), MEBuffer, sizeof(MEBuffer));
    snprintf(m_titleBuffer, sizeof(m_titleBuffer), "ME=%s", MEBuffer);
  }
  return m_titleBuffer;
}

void StatisticGraphController::didBecomeFirstResponder() {
  TestConclusionView::Type t = m_statistic->testPassed() ? TestConclusionView::Type::Success
                                                         : TestConclusionView::Type::Failure;
  GraphDisplayMode m = m_statistic->hypothesisParams()->op() ==
                                   HypothesisParams::ComparisonOperator::Different &&
                               App::app()->subapp() == Data::SubApp::Tests
                           ? GraphDisplayMode::TwoCurve
                           : GraphDisplayMode::OneCurve;
  StatisticGraphView::LegendPosition pos = m_statistic->hypothesisParams()->op() ==
                                          HypothesisParams::ComparisonOperator::Lower
                                      ? StatisticGraphView::LegendPosition::Left
                                      : StatisticGraphView::LegendPosition::Right;
  m_graphView.setMode(m);
  m_graphView.setLegendPosition(pos);
  m_graphView.setType(t);
  m_graphView.setStatistic(m_statistic);
  m_rangeLeft.setMode(m);
  m_rangeRight.setMode(m);
  m_rangeLeft.setStatistic(m_statistic);
  m_rangeRight.setStatistic(m_statistic);
  if (App::app()->subapp() == Data::SubApp::Intervals) {
    m_graphView.intervalConclusionView()->setInterval(m_statistic->estimate(),
                                                      m_statistic->marginOfError());
  }
  m_graphView.reload();
}

}  // namespace Probability
