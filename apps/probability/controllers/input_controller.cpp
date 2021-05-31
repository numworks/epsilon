#include "input_controller.h"

#include <escher/stack_view_controller.h>
#include <poincare/print_float.h>
#include <string.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"
#include "results_controller.h"

using namespace Probability;

InputController::InputController(Escher::StackViewController * parent,
                                 TestResults * resultsController, InputParameters * inputParameters,
                                 Escher::InputEventHandlerDelegate * handler,
                                 Escher::TextFieldDelegate * textFieldDelegate) :
    SelectableListViewPage(parent),
    m_resultsController(resultsController),
    m_inputParameters(inputParameters),
    m_parameterCells{{&m_selectableTableView, handler, textFieldDelegate},
                     {&m_selectableTableView, handler, textFieldDelegate}},
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()) {
  // Initialize cells
  for (int i = 0; i < k_numberOfReusableCells; i++) {
    m_parameterCells[i].setParentResponder(&m_selectableTableView);
    m_parameterCells[i].textField()->setDelegates(handler, textFieldDelegate);
  }
  m_significanceCell.setParentResponder(&m_selectableTableView);
  m_significanceCell.textField()->setDelegates(handler, textFieldDelegate);
  m_significanceCell.setMessage(I18n::Message::Alpha);
  m_significanceCell.setSubLabelMessage(I18n::Message::SignificanceLevel);
}

const char * InputController::title() {
  // H0:<first symbol>=<firstParam> Ha:<first symbol><operator symbol><firstParams>
  m_titleBuffer[0] = 0;
  size_t bufferSize = sizeof(m_titleBuffer);
  const char * H0 = "H0:";
  const char * Ha = "  Ha:";
  const char * eq = "=";  // TODO Must be somewhere else
  const char * symbol = testToTextSymbol(App::app()->snapshot()->data()->test());
  char op[2] = {0, 0};
  op[0] = static_cast<const char>(App::app()->snapshot()->data()->hypothesisParams()->op());
  char paramBuffer[10];
  Poincare::PrintFloat::ConvertFloatToText(
      App::app()->snapshot()->data()->hypothesisParams()->firstParam(), paramBuffer,
      sizeof(paramBuffer), 5, 5, Poincare::Preferences::PrintFloatMode::Decimal);

  strlcat(m_titleBuffer, H0, bufferSize);
  strlcat(m_titleBuffer, symbol, bufferSize);
  strlcat(m_titleBuffer, eq, bufferSize);
  strlcat(m_titleBuffer, paramBuffer, bufferSize);
  strlcat(m_titleBuffer, Ha, bufferSize);
  strlcat(m_titleBuffer, symbol, bufferSize);
  strlcat(m_titleBuffer, op, bufferSize);
  strlcat(m_titleBuffer, paramBuffer, bufferSize);

  return m_titleBuffer;
}

int InputController::typeAtIndex(int i) {
  int numberOfParams = m_inputParameters->numberOfParameters();
  if (i < numberOfParams) {
    return k_parameterCellType;
  }
  if (i == numberOfParams) {
    return k_significanceCellType;
  }
  assert(i == numberOfParams + 1);
  return k_buttonCellType;
}

Escher::HighlightCell * InputController::reusableCell(int i, int type) {
  if (type == k_parameterCellType) {
    return &m_parameterCells[i];
  }
  if (type == k_significanceCellType) {
    // assert(i == 0);
    return &m_significanceCell;
  }
  assert(type == k_buttonCellType);
  return &m_next;
}

void InputController::didBecomeFirstResponder() {
  App::app()->snapshot()->navigation()->setPage(Data::Page::Input);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void InputController::buttonAction() { openPage(m_resultsController); }

void InputController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index < m_inputParameters->numberOfParameters()) {
    MessageTableCellWithEditableTextWithMessage * mCell =
        static_cast<MessageTableCellWithEditableTextWithMessage *>(cell);
    mCell->setMessage(m_inputParameters->paramSymbolAtIndex(index));
    mCell->setSubLabelMessage(m_inputParameters->paramDescriptionAtIndex(index));
  }
}
