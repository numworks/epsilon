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
                                 ResultsController * resultsController,
                                 Statistic * statistic,
                                 Escher::InputEventHandlerDelegate * handler) :
    FloatParameterPage(parent),
    m_statistic(statistic),
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
  if (App::app()->subapp() == Data::SubApp::Tests) {
    // H0:<first symbol>=<firstParam> Ha:<first symbol><operator symbol><firstParams> α=<threshold>
    const char * symbol = testToTextSymbol(App::app()->test());
    char op = HypothesisParams::charForComparisonOp(m_statistic->hypothesisParams()->op());
    char paramBuffer[10];
    defaultParseFloat(m_statistic->hypothesisParams()->firstParam(),
                      paramBuffer,
                      sizeof(paramBuffer));
    if (App::app()->page() == Data::Page::Results || App::app()->page() == Data::Page::Graph) {
      char alphaBuffer[10];
      defaultParseFloat(m_statistic->threshold(), alphaBuffer, sizeof(alphaBuffer));
      sprintf(m_titleBuffer,
              "H0:%s=%s Ha:%s%c%s α=%s",
              symbol,
              paramBuffer,
              symbol,
              op,
              paramBuffer,
              alphaBuffer);
    } else {
      sprintf(m_titleBuffer, "H0:%s=%s Ha:%s%c%s", symbol, paramBuffer, symbol, op, paramBuffer);
    }
  } else {
    const char * symbol = testTypeToText(App::app()->testType());
    const char * text = testToText(App::app()->test());
    sprintf(m_titleBuffer, "%s-interval on %s", symbol, text);
  }
  return m_titleBuffer;
}

ViewController::TitlesDisplay InputController::titlesDisplay() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastTitle;
}

int InputController::typeAtIndex(int i) {
  if (i == m_statistic->indexOfThreshold()) {
    return k_significanceCellType;
  }
  return FloatParameterPage::typeAtIndex(i);
}

void InputController::didBecomeFirstResponder() {
  if (m_statistic->threshold() == -1) {
    m_statistic->initThreshold(App::app()->subapp());
    m_selectableTableView.reloadCellAtLocation(0, m_statistic->indexOfThreshold());
  }
  App::app()->setPage(Data::Page::Input);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void InputController::buttonAction() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    m_statistic->computeTest();
  } else {
    m_statistic->computeInterval();
  }
  openPage(m_resultsController);
}

void InputController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index < m_statistic->indexOfThreshold()) {
    MessageTableCellWithEditableTextWithMessage * mCell =
        static_cast<MessageTableCellWithEditableTextWithMessage *>(cell);
    mCell->setMessage(m_statistic->paramSymbolAtIndex(index));
    mCell->setSubLabelMessage(m_statistic->paramDescriptionAtIndex(index));
  } else if (index == m_statistic->indexOfThreshold()) {
    MessageTableCellWithEditableTextWithMessage * thresholdCell =
        static_cast<MessageTableCellWithEditableTextWithMessage *>(cell);
    I18n::Message name, description;
    if (App::app()->subapp() == Data::SubApp::Tests) {
      name = I18n::Message::GreekAlpha;
      description = I18n::Message::SignificanceLevel;
    } else {
      name = I18n::Message::ConfidenceLevel;
      description = I18n::Message::Default;
    }
    thresholdCell->setMessage(name);
    thresholdCell->setSubLabelMessage(description);
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

bool Probability::InputController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                             const char * text,
                                                             Ion::Events::Event event) {
  bool res = FloatParameterPage::textFieldDidFinishEditing(textField, text, event);
  resetMemoization();
  m_selectableTableView.reloadCellAtLocation(selectedColumn(), selectedRow());
  return res;
}

bool Probability::InputController::textFieldDidHandleEvent(TextField * textField,
                                                           bool returnValue,
                                                           bool textSizeDidChange) {
  if (textField->isEditing()) {
    resetMemoization();
    m_selectableTableView.reloadData();
  }
  return FloatParameterPage::textFieldDidHandleEvent(textField, returnValue, textSizeDidChange);
}

bool Probability::InputController::textFieldDidAbortEditing(TextField * textField) {
  resetMemoization();
  m_selectableTableView.reloadCellAtLocation(selectedColumn(), selectedRow());
  return FloatParameterPage::textFieldDidAbortEditing(textField);
}
