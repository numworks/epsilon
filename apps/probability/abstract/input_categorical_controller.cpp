#include "input_categorical_controller.h"

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/text_helpers.h"
#include <escher/invocation.h>

namespace Probability {

InputCategoricalController::InputCategoricalController(
    StackViewController * parent,
    Escher::ViewController * resultsController,
    Chi2Statistic * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
      Escher::ViewController(parent),
      m_statistic(statistic),
      m_resultsController(resultsController) {
}

bool InputCategoricalController::textFieldShouldFinishEditing(TextField * textField,
                                                              Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Up ||
         event == Ion::Events::Down;
}

bool InputCategoricalController::textFieldDidFinishEditing(TextField * textField,
                                                           const char * text,
                                                           Ion::Events::Event event) {
  // Parse and check significance level
  double p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &p, false, false)) {
    return false;
  }
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
    contentView()->selectViewAtIndex(InputCategoricalView::k_indexOfTable);
  } else {
    contentView()->selectViewAtIndex(InputCategoricalView::k_indexOfNext);
  }
  return true;
}

void InputCategoricalController::didEnterResponderChain(Responder * previousResponder) {
  tableViewController()->recomputeDimensions();
}

void InputCategoricalController::didBecomeFirstResponder() {
  if (m_statistic->threshold() == -1) {
    // Init significance cell
    m_statistic->initThreshold(App::app()->subapp());
  }
  constexpr int bufferSize = Constants::k_shortBufferSize;
  char buffer[bufferSize];
  defaultConvertFloatToText(m_statistic->threshold(), buffer, bufferSize);
  contentView()->setSignificanceCellText(buffer);
  Escher::Container::activeApp()->setFirstResponder(contentView());
}

bool InputCategoricalController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

bool InputCategoricalController::ButtonAction(void * c, void * s) {
  InputCategoricalController * controller = static_cast<InputCategoricalController *>(c);
  if (!controller->m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return false;
  }
  controller->m_statistic->computeTest();
  controller->stackOpenPage(controller->m_resultsController, 1);
  return true;
}

void InputCategoricalController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  int row = tableViewController()->selectableTableView()->selectedRow();
  int col = tableViewController()->selectableTableView()->selectedColumn();
  if (!withinTemporarySelection && previousSelectedCellY != row) {
    // Make contentView scroll to cell
    KDRect cellFrame = KDRect(
        0,  // We'll scroll only vertically
        tableViewController()->tableViewDataSource()->cumulatedHeightFromIndex(row) +
            tableViewController()->selectableTableView()->topMargin(),
        tableViewController()->selectableTableView()->columnWidth(col),
        tableViewController()->tableViewDataSource()->rowHeight(row));

    contentView()->scrollToContentRect(cellFrame);
  }
}

}  // namespace Probability
