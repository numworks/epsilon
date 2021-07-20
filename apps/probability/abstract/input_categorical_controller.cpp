#include "input_categorical_controller.h"

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

InputCategoricalController::InputCategoricalController(StackViewController * parent,
                                                       Page * resultsController,
                                                       Chi2Statistic * statistic,
                                                       InputEventHandlerDelegate * inputEventHandlerDelegate) :
    Page(parent),
    m_statistic(statistic),
    m_resultsController(resultsController),
    m_contentView(this, this, nullptr, inputEventHandlerDelegate, this) {
}

bool InputCategoricalController::textFieldShouldFinishEditing(TextField * textField,
                                                              Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;  // TODO up and down too
}

bool InputCategoricalController::textFieldDidFinishEditing(TextField * textField,
                                                           const char * text,
                                                           Ion::Events::Event event) {
  // Parse significance level
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }
  m_statistic->setThreshold(p);
  // Reparse text
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  defaultParseFloat(p, buffer, bufferSize);
  textField->setText(buffer);
  return true;
}

void InputCategoricalController::didBecomeFirstResponder() {
  if (m_statistic->threshold() == -1) {
    // Init significance cell
    m_statistic->initThreshold(App::app()->subapp());
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    defaultParseFloat(m_statistic->threshold(), buffer, bufferSize);
    m_contentView.setSignificanceCellText(buffer);
  }
  Escher::Container::activeApp()->setFirstResponder(&m_contentView);
}

void InputCategoricalController::buttonAction() {
  m_statistic->computeTest();
  openPage(m_resultsController);
}

}  // namespace Probability
