#include "graph_controller.h"

#include "probability/app.h"

namespace Probability {

GraphController::GraphController(StackViewController * stack) :
    Page(stack), m_rangeLeft(true), m_rangeRight(false), m_graphView(&m_rangeLeft, &m_rangeRight) {
}

void GraphController::didBecomeFirstResponder() {
  // TODO compute mode based on data
  GraphDisplayMode m = GraphDisplayMode::OneCurveView;
  m_graphView.setMode(m);
  m_graphView.setType(TestConclusionView::Type::Success);
  m_graphView.setStatistic(App::app()->snapshot()->data()->statistic());
  m_rangeLeft.setMode(m);
  m_rangeRight.setMode(m);
  m_rangeLeft.setStatistic(App::app()->snapshot()->data()->statistic());
  m_rangeRight.setStatistic(App::app()->snapshot()->data()->statistic());
  m_rangeLeft.setInputParams(App::app()->snapshot()->data()->testInputParams());
  m_rangeRight.setInputParams(App::app()->snapshot()->data()->testInputParams());
  m_graphView.reload();
}

}  // namespace Probability
