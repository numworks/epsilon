#include "hypothesis_controller.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/stack_view_controller.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>
#include <shared/poincare_helpers.h>
#include <string.h>

#include "inference/app.h"
#include "inference/statistic/input_controller.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

HypothesisController::HypothesisController(
    Escher::StackViewController* parent, InputController* inputController,
    InputSlopeController* inputSlopeController,
    InputEventHandlerDelegate* handler, Test* test)
    : Escher::ExplicitSelectableListViewController(parent, this),
      m_inputController(inputController),
      m_inputSlopeController(inputSlopeController),
      m_operatorDataSource(test),
      m_h0(&m_selectableListView, handler, this),
      m_ha(&m_selectableListView, &m_operatorDataSource, this),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<HypothesisController>(
                 &HypothesisController::ButtonAction, this)),
      m_test(test) {
  Poincare::Layout h0 = Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder('H'),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('0'),
          Poincare::VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  Poincare::Layout ha = Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder('H'),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('a'),
          Poincare::VerticalOffsetLayoutNode::VerticalPosition::Subscript));
  m_h0.setLayout(h0);
  m_h0.setSubLabelMessage(I18n::Message::H0Sub);
  m_ha.setLayout(ha);
  m_ha.setSubLabelMessage(I18n::Message::HaSub);
}

const char* HypothesisController::title() {
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(m_test->title()),
                                I18n::translate(I18n::Message::Test));
  return m_titleBuffer;
}

bool HypothesisController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

// TextFieldDelegate

bool HypothesisController::textFieldDidReceiveEvent(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  if (textFieldIsEditable(textField) &&
      (event == Ion::Events::OK || event == Ion::Events::EXE) &&
      !textField->isEditing()) {
    // Remove prefix to edit text
    textField->setText(textField->text() + strlen(symbolPrefix()) +
                       1 /* = symbol */);
  }
  return false;
};

bool HypothesisController::textFieldShouldFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool HypothesisController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, const char* text,
    Ion::Events::Event event) {
  double h0 =
      Shared::PoincareHelpers::ParseAndSimplifyAndApproximateToScalar<double>(
          text, AppsContainerHelper::sharedAppsContainerGlobalContext());
  // Check
  if (std::isnan(h0) || !m_test->isValidH0(h0)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }

  m_test->hypothesisParams()->setFirstParam(h0);
  loadHypothesisParam();
  m_selectableListView.selectCell(k_indexOfHa);
  return true;
}

bool HypothesisController::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  // Reload params to add "p=..."
  loadHypothesisParam();
  return true;
}

void HypothesisController::onDropdownSelected(int selectedRow) {
  m_test->hypothesisParams()->setComparisonOperator(
      ComparisonOperatorPopupDataSource::OperatorTypeForRow(selectedRow));
}

const char* HypothesisController::symbolPrefix() {
  return m_test->hypothesisSymbol();
}

HighlightCell* HypothesisController::cell(int index) {
  HighlightCell* cells[] = {&m_h0, &m_ha, &m_next};
  return cells[index];
}

void HypothesisController::didBecomeFirstResponder() {
  selectCell(0);
  m_ha.dropdown()->selectRow(
      static_cast<int>(m_test->hypothesisParams()->comparisonOperator()));
  m_ha.dropdown()->init();
  loadHypothesisParam();
  resetMemoization();
  m_selectableListView.reloadData(true);
}

bool HypothesisController::ButtonAction(HypothesisController* controller,
                                        void* s) {
  if (controller->m_test->significanceTestType() ==
      SignificanceTestType::Slope) {
    controller->stackOpenPage(controller->m_inputSlopeController);
  } else {
    controller->stackOpenPage(controller->m_inputController);
  }
  return true;
}

void HypothesisController::loadHypothesisParam() {
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed", symbolPrefix(),
      m_test->hypothesisParams()->firstParam(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits);
  m_h0.setAccessoryText(buffer);
  m_ha.reload();
  resetMemoization();
  m_selectableListView.reloadData();
}

}  // namespace Inference
