#include "graph_controller.h"

#include "probability/app.h"

namespace Probability {

GraphController::GraphController(StackViewController * stack) :
    Page(stack), m_rangeLeft(false), m_rangeRight(true), m_graphView(&m_rangeLeft, &m_rangeRight) {
}

void GraphController::didBecomeFirstResponder() {
  m_graphView.computeMode();
  m_graphView.setType(TestConclusionView::Type::Success);
  m_graphView.setStatistic(App::app()->snapshot()->data()->statistic());
  m_rangeLeft.setStatistic(App::app()->snapshot()->data()->statistic());
  m_rangeLeft.setInputParams(App::app()->snapshot()->data()->testInputParams());
  m_rangeRight.setStatistic(App::app()->snapshot()->data()->statistic());
  m_rangeRight.setInputParams(App::app()->snapshot()->data()->testInputParams());
  m_graphView.reload();
}

}  // namespace Probability
