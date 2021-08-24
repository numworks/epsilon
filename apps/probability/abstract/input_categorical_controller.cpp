#include "input_categorical_controller.h"

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

InputCategoricalController::InputCategoricalController(
    StackViewController * parent,
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
  return event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Up ||
         event == Ion::Events::Down;
}

bool InputCategoricalController::textFieldDidFinishEditing(TextField * textField,
                                                           const char * text,
                                                           Ion::Events::Event event) {
  // Parse and check significance level
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false) ||
      !m_statistic->isValidParamAtIndex(m_statistic->indexOfThreshold(), p)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setThreshold(p);
  // Reparse text
  constexpr int bufferSize = Constants::k_shortBufferSize;
  char buffer[bufferSize];
  defaultParseFloat(p, buffer, bufferSize);
  textField->setText(buffer);
  if (event == Ion::Events::Up) {
    m_contentView.selectView(InputCategoricalView::k_indexOfTable);
  } else {
    m_contentView.selectView(InputCategoricalView::k_indexOfNext);
  }
  return true;
}

void InputCategoricalController::didBecomeFirstResponder() {
  if (m_statistic->threshold() == -1) {
    // Init significance cell
    m_statistic->initThreshold(App::app()->subapp());
    constexpr int bufferSize = Constants::k_shortBufferSize;
    char buffer[bufferSize];
    defaultParseFloat(m_statistic->threshold(), buffer, bufferSize);
    m_contentView.setSignificanceCellText(buffer);
  }
  Escher::Container::activeApp()->setFirstResponder(&m_contentView);
}

bool InputCategoricalController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackViewController()->pop();
    return true;
  }
  return false;
}

bool InputCategoricalController::buttonAction() {
  if (!m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return false;
  }
  m_statistic->computeTest();
  openPage(m_resultsController);
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
    // Make m_contentView scroll to cell
    KDRect cellFrame = KDRect(
        tableViewController()->tableViewDataSource()->cumulatedWidthFromIndex(col),
        tableViewController()->tableViewDataSource()->cumulatedHeightFromIndex(row) +
            tableViewController()->selectableTableView()->topMargin(),
        tableViewController()->selectableTableView()->columnWidth(col),
        tableViewController()->tableViewDataSource()->rowHeight(row));

    m_contentView.scrollToContentRect(cellFrame);
  }
}

}  // namespace Probability
