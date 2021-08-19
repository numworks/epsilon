#include "input_controller.h"

#include <escher/stack_view_controller.h>
#include <poincare/print_float.h>
#include <string.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"
#include "results_controller.h"

using namespace Probability;

KDColor InputController::s_titleColor = KDColor::RGB24(0xA3A0AB);

InputController::InputController(Escher::StackViewController * parent,
                                 ResultsController * resultsController,
                                 Statistic * statistic,
                                 Escher::InputEventHandlerDelegate * handler) :
    FloatParameterPage(parent), m_statistic(statistic), m_resultsController(resultsController) {
  // Initialize cells
  for (int i = 0; i < k_numberOfReusableCells; i++) {
    m_parameterCells[i].setParentResponder(&m_selectableTableView);
    m_parameterCells[i].setDelegates(handler, this);
  }
  m_significanceCell.setParentResponder(&m_selectableTableView);
  m_significanceCell.innerCell()->setDelegates(handler, this);
  m_significanceCell.innerCell()->setMessage(I18n::Message::Alpha);
  m_significanceCell.innerCell()->setSubLabelMessage(I18n::Message::SignificanceLevel);
}

const char * InputController::title() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    // H0:<first symbol>=<firstParam> Ha:<first symbol><operator symbol><firstParams> α=<threshold>
    const char * symbol = testToTextSymbol(App::app()->test());
    const char * op = HypothesisParams::strForComparisonOp(m_statistic->hypothesisParams()->op());
    char paramBuffer[10];
    defaultParseFloat(m_statistic->hypothesisParams()->firstParam(),
                      paramBuffer,
                      sizeof(paramBuffer));
    if (App::app()->page() == Data::Page::Results || App::app()->page() == Data::Page::Graph) {
      char alphaBuffer[10];
      defaultParseFloat(m_statistic->threshold(), alphaBuffer, sizeof(alphaBuffer));
      snprintf(m_titleBuffer,
               sizeof(m_titleBuffer),
               "H0:%s=%s Ha:%s%s%s α=%s",
               symbol,
               paramBuffer,
               symbol,
               op,
               paramBuffer,
               alphaBuffer);
    } else {
      snprintf(m_titleBuffer,
               sizeof(m_titleBuffer),
               "H0:%s=%s Ha:%s%s%s",
               symbol,
               paramBuffer,
               symbol,
               op,
               paramBuffer);
    }
  } else {
    const char * symbol = testTypeToText(App::app()->testType());
    const char * text = testToText(App::app()->test());
    const char * format = I18n::translate(I18n::Message::InputControllerIntervalTitleFormat);
    snprintf(m_titleBuffer, sizeof(m_titleBuffer), format, symbol, text);
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
  selectCellAtLocation(0, 0);
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void InputController::buttonAction() {
  if (!m_statistic->validateInputs()) {
    App::app()->displayWarning(I18n::Message::InvalidInputs);
    return;
  }
  if (App::app()->subapp() == Data::SubApp::Tests) {
    m_statistic->computeTest();
  } else {
    m_statistic->computeInterval();
  }
  openPage(m_resultsController);
}

void InputController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index < m_statistic->indexOfThreshold()) {
    LayoutCellWithEditableTextWithMessage * mCell =
        static_cast<LayoutCellWithEditableTextWithMessage *>(cell);
    mCell->setLayout(m_statistic->paramSymbolAtIndex(index));
    mCell->setSubLabelMessage(m_statistic->paramDescriptionAtIndex(index));
  } else if (index == m_statistic->indexOfThreshold()) {
    MessageTableCellWithSeparator * thresholdCell = static_cast<MessageTableCellWithSeparator *>(
        cell);
    I18n::Message name, description;
    if (App::app()->subapp() == Data::SubApp::Tests) {
      name = I18n::Message::GreekAlpha;
      description = I18n::Message::SignificanceLevel;
    } else {
      name = I18n::Message::ConfidenceLevel;
      description = I18n::Message::Default;
    }
    thresholdCell->innerCell()->setMessage(name);
    thresholdCell->innerCell()->setSubLabelMessage(description);
  }
  FloatParameterPage::willDisplayCellForIndex(cell, index);
}

Escher::HighlightCell * InputController::reusableParameterCell(int index, int type) {
  if (type == k_parameterCellType) {
    assert(index >= 0 && index < k_numberOfReusableCells);
    return &(m_parameterCells[index]);
  }
  assert(type == k_significanceCellType);
  return &m_significanceCell;
}

bool Probability::InputController::handleEvent(Ion::Events::Event event) {
  if ((App::app()->subapp() == Data::SubApp::Intervals) && (event == Ion::Events::Left)) {
    stackViewController()->pop();
    return true;
  }
  return false;
}

bool Probability::InputController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                             const char * text,
                                                             Ion::Events::Event event) {
  bool res = FloatParameterPage::textFieldDidFinishEditing(textField, text, event);
  m_selectableTableView.reloadCellAtLocation(selectedColumn(), selectedRow());
  return res;
}

bool Probability::InputController::isCellEditing(Escher::HighlightCell * cell, int index) {
  if (index == m_statistic->indexOfThreshold()) {
    return static_cast<MessageTableCellWithSeparator *>(cell)
        ->innerCell()
        ->textField()
        ->isEditing();
  }
  return static_cast<LayoutCellWithEditableTextWithMessage *>(cell)->textField()->isEditing();
}

void Probability::InputController::setTextInCell(Escher::HighlightCell * cell,
                                                 const char * text,
                                                 int index) {
  if (index == m_statistic->indexOfThreshold()) {
    static_cast<MessageTableCellWithSeparator *>(cell)->innerCell()->textField()->setText(text);
  } else {
    static_cast<LayoutCellWithEditableTextWithMessage *>(cell)->textField()->setText(text);
  }
}

bool Probability::InputController::setParameterAtIndex(int parameterIndex, float f) {
  if (!m_statistic->isValidParamAtIndex(parameterIndex, f)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_statistic->setParamAtIndex(parameterIndex, f);
  return true;
}
