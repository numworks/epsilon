#include "hypothesis_controller.h"

#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/preferences.h>
#include <poincare/vertical_offset_layout.h>
#include <shared/poincare_helpers.h>
#include <string.h>

#include <new>

#include "input_controller.h"
#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

HypothesisController::HypothesisController(Escher::StackViewController * parent,
                                           InputController * inputController,
                                           InputEventHandlerDelegate * handler,
                                           Statistic * statistic) :
    SelectableListViewPage(parent),
    m_inputController(inputController),
    m_operatorDataSource(statistic->hypothesisParams()),
    m_h0(&m_selectableTableView, handler, this),
    m_ha(&m_selectableTableView, &m_operatorDataSource, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_statistic(statistic) {
  Poincare::Layout h0 = Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder('H', KDFont::LargeFont),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('0', KDFont::LargeFont),
          Poincare::VerticalOffsetLayoutNode::Position::Subscript));
  Poincare::Layout ha = Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder('H', KDFont::LargeFont),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('a', KDFont::LargeFont),
          Poincare::VerticalOffsetLayoutNode::Position::Subscript));
  m_h0.setLayout(h0);
  m_h0.setSubLabelMessage(I18n::Message::H0Sub);
  m_ha.setLayout(ha);
  m_ha.setSubLabelMessage(I18n::Message::HaSub);
}

const char * Probability::HypothesisController::title() {
  const char * testType = testTypeToText(App::app()->testType());
  const char * test = testToText(App::app()->test());
  const char * format = I18n::translate(I18n::Message::HypothesisControllerTitleFormat);
  snprintf(m_titleBuffer, sizeof(m_titleBuffer), format, testType, test);
  return m_titleBuffer;
}

bool Probability::HypothesisController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackViewController()->pop();
    return true;
  }
  return false;
}

// TextFieldDelegate
bool Probability::HypothesisController::textFieldDidReceiveEvent(Escher::TextField * textField,
                                                                 Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    // Remove prefix to edit text
    textField->setText(textField->text() + strlen(symbolPrefix()) + 1 /* = symbol */);
  }
  return false;
};

bool Probability::HypothesisController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                     Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  }
  return false;
}

bool Probability::HypothesisController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                  const char * text,
                                                                  Ion::Events::Event event) {
  float h0 = Shared::PoincareHelpers::ApproximateToScalar<float>(
      text,
      AppsContainer::sharedAppsContainer()->globalContext());
  // Check
  if (std::isnan(h0) || !m_statistic->isValidH0(h0)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }

  m_statistic->hypothesisParams()->setFirstParam(h0);
  loadHypothesisParam();
  m_selectableTableView.selectCellAtLocation(0, k_indexOfHa);
  return true;
}

bool Probability::HypothesisController::textFieldDidAbortEditing(TextField * textField) {
  // Reload params to add "p=..."
  loadHypothesisParam();
  return true;
}

void Probability::HypothesisController::onDropdownSelected(int selectedRow) {
  HypothesisParams::ComparisonOperator op;
  switch (selectedRow) {
    case 0:
      op = HypothesisParams::ComparisonOperator::Lower;
      break;
    case 1:
      op = HypothesisParams::ComparisonOperator::Different;
      break;
    case 2:
      op = HypothesisParams::ComparisonOperator::Higher;
      break;
  }
  m_statistic->hypothesisParams()->setOp(op);
}

const char * Probability::HypothesisController::symbolPrefix() {
  return testToTextSymbol(App::app()->test());
}

HighlightCell * HypothesisController::reusableCell(int i, int type) {
  switch (i) {
    case k_indexOfH0:
      return &m_h0;
    case k_indexOfHa:
      return &m_ha;
    default:
      assert(i == k_indexOfNext);
      return &m_next;
  }
}

void HypothesisController::didBecomeFirstResponder() {
  App::app()->setPage(Data::Page::Hypothesis);
  selectCellAtLocation(0, 0);
  m_ha.dropdown()->init();
  loadHypothesisParam();
  resetMemoization();
  m_selectableTableView.reloadData(true);
}

bool HypothesisController::buttonAction() {
  openPage(m_inputController);
  return true;
}

void HypothesisController::loadHypothesisParam() {
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  strlcpy(buffer, symbolPrefix(), bufferSize);
  int offset = strlen(symbolPrefix());
  buffer[offset++] = '=';
  float p = m_statistic->hypothesisParams()->firstParam();
  defaultParseFloat(p, buffer + offset, bufferSize - offset);

  m_h0.setAccessoryText(buffer);
  m_ha.reload();
  resetMemoization();  // TODO only when m_ha changes size ?
  m_selectableTableView.reloadData();
}
