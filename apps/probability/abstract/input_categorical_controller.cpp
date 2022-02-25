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
  return handleEditedValue(m_statistic->indexOfThreshold(), p, textField, event, InputCategoricalView::k_indexOfTable, contentView()->indexOfNext());
}

void InputCategoricalController::didEnterResponderChain(Responder * previousResponder) {
  tableViewController()->recomputeDimensions();
}

void InputCategoricalController::didBecomeFirstResponder() {
  if (m_statistic->threshold() == -1) {
    // Init significance cell
    m_statistic->initThreshold(App::app()->subapp());
  }
  contentView()->updateSignificanceCell(m_statistic);
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
  controller->stackOpenPage(controller->m_resultsController);
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

bool InputCategoricalController::handleEditedValue(int i, double p, TextField * textField, Ion::Events::Event event, int indexIdUp, int indexOtherwise) {
  if (!m_statistic->isValidParamAtIndex(i, p)) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
  }
  m_statistic->setParamAtIndex(i, p);
  contentView()->setTextFieldText(p, textField);
  contentView()->selectViewAtIndex(event == Ion::Events::Up ? indexIdUp : indexOtherwise);
  return true;
}


}  // namespace Probability
