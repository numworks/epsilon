#include "graph_controller.h"

#include "probability/app.h"

namespace Probability {

GraphController::GraphController(StackViewController * stack) :
    Page(stack), m_rangeLeft(true), m_rangeRight(false), m_graphView(&m_rangeLeft, &m_rangeRight) {
}

void GraphController::didBecomeFirstResponder() {
  // TODO compute mode based on data
  Statistic * statistic = App::app()->snapshot()->data()->statistic();
  TestConclusionView::Type t = statistic->testPassed() ? TestConclusionView::Type::Success
                                                       : TestConclusionView::Type::Failure;
  GraphDisplayMode m = GraphDisplayMode::OneCurveView;
  m_graphView.setMode(m);
  m_graphView.setType(t);
  m_graphView.setStatistic(statistic);
  m_rangeLeft.setMode(m);
  m_rangeRight.setMode(m);
  m_rangeLeft.setStatistic(statistic);
  m_rangeRight.setStatistic(statistic);
  m_graphView.reload();
}

}  // namespace Probability
