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
                                 ResultsController * resultsController, InputParameters * inputParameters,
                                 Escher::InputEventHandlerDelegate * handler) :
    FloatParameterPage(parent),
    m_inputParameters(inputParameters),
    m_resultsController(resultsController),
    m_parameterCells{{&m_selectableTableView, handler, this},
                     {&m_selectableTableView, handler, this}} {
  // Initialize cells
  for (int i = 0; i < k_numberOfReusableCells; i++) {
    m_parameterCells[i].setParentResponder(&m_selectableTableView);
    m_parameterCells[i].textField()->setDelegates(handler, this);
  }
  m_significanceCell.setParentResponder(&m_selectableTableView);
  m_significanceCell.textField()->setDelegates(handler, this);
  m_significanceCell.setMessage(I18n::Message::Alpha);
  m_significanceCell.setSubLabelMessage(I18n::Message::SignificanceLevel);
}

const char * InputController::title() {
  size_t bufferSize = sizeof(m_titleBuffer);
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    // H0:<first symbol>=<firstParam> Ha:<first symbol><operator symbol><firstParams>
    m_titleBuffer[0] = 0;
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
  } else {
    strlcpy(m_titleBuffer, "z-interval bla...", bufferSize);
  }

  return m_titleBuffer;
}


ViewController::TitlesDisplay InputController::titlesDisplay() {
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastTitles;
}

int InputController::typeAtIndex(int i) {
  if (i == m_inputParameters->numberOfParameters()) {
    return k_significanceCellType;
  }
  return FloatParameterPage::typeAtIndex(i);
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

void InputController::buttonAction() {
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    App::app()->snapshot()->data()->statistic()->computeTest();
  } else {
    App::app()->snapshot()->data()->statistic()->computeInterval();
  }
  openPage(m_resultsController);
}

void InputController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index < m_inputParameters->numberOfParameters()) {
    MessageTableCellWithEditableTextWithMessage * mCell =
        static_cast<MessageTableCellWithEditableTextWithMessage *>(cell);
    mCell->setMessage(m_inputParameters->paramSymbolAtIndex(index));
    mCell->setSubLabelMessage(m_inputParameters->paramDescriptionAtIndex(index));
  }
  FloatParameterPage::willDisplayCellForIndex(cell, index);
}

Escher::HighlightCell * InputController::reusableParameterCell(int index, int type) {
  if (type == k_parameterCellType) {
    return &(m_parameterCells[index]);
  }
  assert(type == k_significanceCellType);
  return &m_significanceCell;
}

