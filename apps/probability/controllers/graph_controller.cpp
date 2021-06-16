#include "graph_controller.h"

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

GraphController::GraphController(StackViewController * stack) :
    Page(stack),
    m_rangeLeft(m_graphView.curveViewLeft(), true),
    m_rangeRight(m_graphView.curveViewRight(), false),
    m_graphView(&m_rangeLeft, &m_rangeRight) {
}

const char * GraphController::title() {
  char zBuffer[10];
  char pBuffer[10];
  Statistic * statistic = App::app()->snapshot()->data()->statistic();
  Poincare::PrintFloat::ConvertFloatToText(statistic->testCriticalValue(), zBuffer, sizeof(zBuffer),
                                           5, 3, Poincare::Preferences::PrintFloatMode::Decimal);
  Poincare::PrintFloat::ConvertFloatToText(statistic->pValue(), pBuffer, sizeof(pBuffer), 10, 3,
                                           Poincare::Preferences::PrintFloatMode::Decimal);

  sprintf(m_titleBuffer, "z=%s p-value=%s", zBuffer, pBuffer);
  return m_titleBuffer;
}

void GraphController::didBecomeFirstResponder() {
  Statistic * statistic = App::app()->snapshot()->data()->statistic();
  TestConclusionView::Type t = statistic->testPassed() ? TestConclusionView::Type::Success
                                                       : TestConclusionView::Type::Failure;
  GraphDisplayMode m =
      statistic->hypothesisParams()->op() == HypothesisParams::ComparisonOperator::Different &&
              App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests
          ? GraphDisplayMode::TwoCurveViews
          : GraphDisplayMode::OneCurveView;
  m_graphView.setMode(m);
  m_graphView.setType(t);
  m_graphView.setStatistic(statistic);
  m_rangeLeft.setMode(m);
  m_rangeRight.setMode(m);
  m_rangeLeft.setStatistic(statistic);
  m_rangeRight.setStatistic(statistic);
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Intervals) {
    m_graphView.intervalConclusionView()->setInterval(statistic->estimate(),
                                                      statistic->marginOfError());
  }
  m_graphView.reload();
}

}  // namespace Probability
