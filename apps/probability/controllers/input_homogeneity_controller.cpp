#include "input_homogeneity_controller.h"

#include "probability/app.h"

using namespace Probability;

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent,
    ResultsHomogeneityController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    InputCategoricalController(parent,
                               homogeneityResultsController,
                               statistic,
                               inputEventHandlerDelegate),
    m_tableController(&m_contentView, statistic, inputEventHandlerDelegate, &m_contentView, this) {
  m_contentView.setTableView(&m_tableController);
}

void Probability::InputHomogeneityController::didBecomeFirstResponder() {
  App::app()->setPage(Data::Page::InputHomogeneity);
  InputCategoricalController::didBecomeFirstResponder();
}
