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
    if (!m_statistic->isValidParamAtIndex(m_statistic->indexOfDegreeOfFreedom(), p)) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_statistic->setDegreeOfFreedom(p);
    // Reparse text
    contentView()->setTextFieldText(p, textField);
    if (event == Ion::Events::Up) {
      contentView()->selectViewAtIndex(InputGoodnessView::k_indexOfTable);
    } else {
      contentView()->selectViewAtIndex(InputGoodnessView::k_indexOfSignificance);
    }
  } else {
    assert(selectedView == InputGoodnessView::k_indexOfSignificance);
    // Parse and check significance level
    if (!m_statistic->isValidParamAtIndex(m_statistic->indexOfThreshold(), p)) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_statistic->setThreshold(p);
    contentView()->setTextFieldText(p, textField);
    if (event == Ion::Events::Up) {
      contentView()->selectViewAtIndex(InputGoodnessView::k_indexOfDegreeOfFreedom);
    } else {
      contentView()->selectViewAtIndex(InputGoodnessView::k_indexOfNext);
    }
  }
  return true;
}

void Probability::InputGoodnessController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::InputGoodness);
  m_contentView.updateDegreeOfFreedomCell(m_statistic);
  InputCategoricalController::didBecomeFirstResponder();
}
