#include "input_goodness_controller.h"

#include <escher/input_event_handler_delegate.h>

#include "probability/app.h"

using namespace Probability;

InputGoodnessController::InputGoodnessController(
    StackViewController * parent,
    ResultsController * resultsController,
    GoodnessStatistic * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
      InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
      m_tableController(&m_contentView,
                        statistic,
                        &m_contentView,
                        this),
      m_contentView(this, Escher::Invocation(&InputCategoricalController::ButtonAction, this), nullptr, inputEventHandlerDelegate, this) {
  m_contentView.setTableView(&m_tableController);
}

void Probability::InputGoodnessController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::InputGoodness);
  InputCategoricalController::didBecomeFirstResponder();
}
