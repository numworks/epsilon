#include "hypothesis_controller.h"

#include <apps/apps_container.h>
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

#include "inference/statistic/input_controller.h"
#include "inference/app.h"
#include "inference/text_helpers.h"

using namespace Inference;

HypothesisController::HypothesisController(Escher::StackViewController * parent,
                                           InputController * inputController,
                                           InputSlopeController * inputSlopeController,
                                           InputEventHandlerDelegate * handler,
                                           Test * test) :
      Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>(parent),
      m_inputController(inputController),
      m_inputSlopeController(inputSlopeController),
      m_operatorDataSource(test),
      m_h0(&m_selectableTableView, handler, this),
      m_ha(&m_selectableTableView, &m_operatorDataSource, this),
      m_next(&m_selectableTableView, I18n::Message::Next, Escher::Invocation(&HypothesisController::ButtonAction, this)),
      m_test(test) {
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

const char * HypothesisController::title() {
  Poincare::Print::customPrintf(m_titleBuffer,
           sizeof(m_titleBuffer),
           I18n::translate(m_test->title()),
           I18n::translate(I18n::Message::Test));
  return m_titleBuffer;
}

bool HypothesisController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

// TextFieldDelegate

bool HypothesisController::textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) {
  if (selectedRow() == 0 && m_test->significanceTestType() == SignificanceTestType::Slope) {
    /* Forbid the edition of Test t for slope hypothesis by catching the event
     * before being given to the textfield. */
    m_selectableTableView.handleEvent(event);
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    // Remove prefix to edit text
    textField->setText(textField->text() + strlen(symbolPrefix()) + 1 /* = symbol */);
  }
  return false;
};

bool HypothesisController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                     Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool HypothesisController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                  const char * text,
                                                                  Ion::Events::Event event) {
  double h0 = Shared::PoincareHelpers::ApproximateToScalar<double>(
      text,
      AppsContainer::sharedAppsContainer()->globalContext());
  // Check
  if (std::isnan(h0) || !m_test->isValidH0(h0)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }

  m_test->hypothesisParams()->setFirstParam(h0);
  loadHypothesisParam();
  m_selectableTableView.selectCellAtLocation(0, k_indexOfHa);
  return true;
}

bool HypothesisController::textFieldDidAbortEditing(TextField * textField) {
  // Reload params to add "p=..."
  loadHypothesisParam();
  return true;
}

void HypothesisController::onDropdownSelected(int selectedRow) {
  HypothesisParams::ComparisonOperator op;
  switch (selectedRow) {
    case 0:
      op = HypothesisParams::ComparisonOperator::Lower;
      break;
    case 1:
      op = HypothesisParams::ComparisonOperator::Different;
      break;
    default:
      assert(selectedRow == 2);
      op = HypothesisParams::ComparisonOperator::Higher;
      break;
  }
  m_test->hypothesisParams()->setComparisonOperator(op);
}

const char * HypothesisController::symbolPrefix() {
  return m_test->hypothesisSymbol();
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
  selectCellAtLocation(0, 0);
  m_ha.dropdown()->selectRow(
      static_cast<int>(m_test->hypothesisParams()->comparisonOperator()));
  m_ha.dropdown()->init();
  loadHypothesisParam();
  resetMemoization();
  m_selectableTableView.reloadData(true);
}

bool HypothesisController::ButtonAction(void * c, void * s) {
  HypothesisController * controller = static_cast<HypothesisController *>(c);
  if (controller->m_test->significanceTestType() == SignificanceTestType::Slope) {
    controller->stackOpenPage(controller->m_inputSlopeController);
  } else {
    controller->stackOpenPage(controller->m_inputController);
  }
  return true;
}

void HypothesisController::loadHypothesisParam() {
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::customPrintf(buffer, bufferSize, "%s=%*.*ed", symbolPrefix(), m_test->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  m_h0.setAccessoryText(buffer);
  m_ha.reload();
  resetMemoization();
  m_selectableTableView.reloadData();
}
