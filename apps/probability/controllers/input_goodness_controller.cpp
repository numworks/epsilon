#include "input_goodness_controller.h"

#include <escher/input_event_handler_delegate.h>

#include "probability/app.h"

using namespace Probability;

InputGoodnessController::InputGoodnessController(
    StackViewController * parent,
    ResultsController * resultsController,
    GoodnessTest * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
      InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
      m_tableController(&m_contentView,
                        statistic,
                        &m_contentView,
                        this,
                        &m_contentView),
      m_contentView(this, Escher::Invocation(&InputCategoricalController::ButtonAction, this), nullptr, inputEventHandlerDelegate, this) {
  m_contentView.setTableView(&m_tableController);
}

// TODO : Factorize with InputCategoricalController::textFieldDidFinishEditing
bool InputGoodnessController::textFieldDidFinishEditing(TextField * textField,
                                                           const char * text,
                                                           Ion::Events::Event event) {
  int selectedView = contentView()->selectedView();
  double p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &p, false, false)) {
    return false;
  }
  if (selectedView == InputGoodnessView::k_indexOfDegreeOfFreedom) {
    // Parse and check degrees of freedom
    return handleEditedValue(m_statistic->indexOfDegreeOfFreedom(), p, textField, event, InputGoodnessView::k_indexOfTable, InputGoodnessView::k_indexOfSignificance);
  }
  assert(selectedView == InputGoodnessView::k_indexOfSignificance);
  // Parse and check significance level
  return handleEditedValue(m_statistic->indexOfThreshold(), p, textField, event, InputGoodnessView::k_indexOfDegreeOfFreedom, InputGoodnessView::k_indexOfNext);
}

void Probability::InputGoodnessController::didBecomeFirstResponder() {
  m_contentView.updateDegreeOfFreedomCell(m_statistic->degreeOfFreedom());
  InputCategoricalController::didBecomeFirstResponder();
}
