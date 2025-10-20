#include "hypothesis_editable_controller.h"

#include <poincare/layout.h>
#include <poincare/print.h>
#include <poincare/user_expression.h>
#include <shared/poincare_helpers.h>
#include <string.h>

#include "../controller_container.h"
#include "inference/app.h"

namespace Inference {

HypothesisEditableController::HypothesisEditableController(
    Escher::StackViewController* parent,
    ControllerContainer* controllerContainer, SignificanceTest* test)
    : HypothesisController(parent, controllerContainer, test),
      m_operatorDataSource(test),
      m_h0(&m_selectableListView, this),
      m_haDropdown(&m_selectableListView, &m_operatorDataSource, this) {
  m_h0.label()->setLayout("H"_l ^ KSubscriptL("0"_l));
  m_h0.subLabel()->setMessage(I18n::Message::H0Sub);
  m_ha.label()->setLayout("H"_l ^ KSubscriptL("a"_l));
  m_ha.subLabel()->setMessage(I18n::Message::HaSub);
  m_ha.accessory()->setDropdown(&m_haDropdown);
}

// TextFieldDelegate

bool HypothesisEditableController::textFieldDidReceiveEvent(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  // If the textField is not editable, then it shouldn't enter responder chain.
  assert(selectedRow() == 0 && m_h0.textFieldIsEditable(textField));
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      !textField->isEditing()) {
    // Remove prefix to edit text
    textField->setText(textField->text() + strlen(symbolPrefix()) +
                       1 /* = symbol */);
  }
  return false;
}

bool HypothesisEditableController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  double h0 =
      Poincare::UserExpression::ParseAndSimplifyAndApproximateToRealScalar<
          double>(textField->draftText(),
                  Shared::GlobalContextAccessor::Context(),
                  GlobalPreferences::SharedGlobalPreferences()->complexFormat(),
                  GlobalPreferences::SharedGlobalPreferences()->angleUnit());
  // Check
  if (std::isnan(h0) || !m_test->isValidH0(h0)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }

  m_test->hypothesis()->m_h0 = h0;
  loadHypothesisParam();
  m_selectableListView.selectCell(k_indexOfHa);
  return true;
}

void HypothesisEditableController::textFieldDidAbortEditing(
    Escher::AbstractTextField* textField) {
  // Reload params to add "p=..."
  loadHypothesisParam();
}

void HypothesisEditableController::onDropdownSelected(int selectedRow) {
  m_test->hypothesis()->m_alternative =
      ComparisonOperatorPopupDataSource::OperatorTypeForRow(selectedRow);
}

const char* HypothesisEditableController::symbolPrefix() const {
  return m_test->hypothesisSymbol();
}

const Escher::HighlightCell* HypothesisEditableController::cell(int row) const {
  const Escher::HighlightCell* cells[] = {&m_h0, &m_ha, &m_next};
  return cells[row];
}

void HypothesisEditableController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    selectRow(0);
    m_haDropdown.selectRow(
        static_cast<int>(m_test->hypothesis()->m_alternative));
    m_haDropdown.init();
    loadHypothesisParam();
  }
  HypothesisController::handleResponderChainEvent(event);
}

void HypothesisEditableController::loadHypothesisParam() {
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed", symbolPrefix(),
      m_test->hypothesis()->m_h0,
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits);
  m_h0.textField()->setText(buffer);
  m_operatorDataSource.updateMessages();
  m_haDropdown.reloadCell();
  m_selectableListView.reloadData();
}

}  // namespace Inference
