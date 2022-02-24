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
    // TODO : Implement this validity check in Probability::Chi2Statistic
    if (p != std::round(p) || p < 1.0) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    m_statistic->setDegreeOfFreedom(p);
    // Reparse text
    constexpr int bufferSize = Constants::k_shortBufferSize;
    char buffer[bufferSize];
    defaultConvertFloatToText(p, buffer, bufferSize);
    textField->setText(buffer);
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
    // Reparse text
    constexpr int bufferSize = Constants::k_shortBufferSize;
    char buffer[bufferSize];
    defaultConvertFloatToText(p, buffer, bufferSize);
    textField->setText(buffer);
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
  constexpr int bufferSize = Constants::k_shortBufferSize;
  char buffer[bufferSize];
  int degreeOfFreedom = m_statistic->degreeOfFreedom();
  if (degreeOfFreedom >= 0) {
    defaultConvertFloatToText(degreeOfFreedom, buffer, bufferSize);
  } else {
    buffer[0] = 0;
  }
  m_contentView.setDegreeOfFreedomCellText(buffer);
  InputCategoricalController::didBecomeFirstResponder();
}
