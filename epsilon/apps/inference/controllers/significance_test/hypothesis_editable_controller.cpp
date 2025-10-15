#include "hypothesis_editable_controller.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <escher/invocation.h>
#include <escher/stack_view_controller.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/print.h>
#include <poincare/user_expression.h>
#include <shared/poincare_helpers.h>
#include <string.h>

#include "inference/app.h"
#include "inference/controllers/input_controller.h"
#include "inference/models/aliases.h"

using namespace Escher;

namespace Inference {

HypothesisEditableController::HypothesisEditableController(
    Escher::StackViewController* parent, InputController* inputController,
    InputStoreController* inputStoreController,
    DatasetController* datasetController, SignificanceTest* test)
    : Escher::ExplicitSelectableListViewController(parent),
      m_inputController(inputController),
      m_inputStoreController(inputStoreController),
      m_datasetController(datasetController),
      m_operatorDataSource(test),
      m_h0(&m_selectableListView, this),
      m_haDropdown(&m_selectableListView, &m_operatorDataSource, this),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<HypothesisEditableController>(
                 &HypothesisEditableController::ButtonAction, this),
             ButtonCell::Style::EmbossedLight),
      m_test(test) {
  m_h0.label()->setLayout("H"_l ^ KSubscriptL("0"_l));
  m_h0.subLabel()->setMessage(I18n::Message::H0Sub);
  m_haWithDropdown.label()->setLayout("H"_l ^ KSubscriptL("a"_l));
  m_haNoDropdown.label()->setLayout("H"_l ^ KSubscriptL("a"_l));
  m_haWithDropdown.subLabel()->setMessage(I18n::Message::HaSub);
  m_haNoDropdown.subLabel()->setMessage(I18n::Message::HaSub);
  m_ha = &m_haNoDropdown;
}

const char* HypothesisEditableController::title() const {
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(m_test->title()),
                                I18n::translate(I18n::Message::Test));
  return m_titleBuffer;
}

bool HypothesisEditableController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
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
    AbstractTextField* textField) {
  // Reload params to add "p=..."
  loadHypothesisParam();
}

void HypothesisEditableController::onDropdownSelected(int selectedRow) {
  m_test->hypothesis()->m_alternative =
      ComparisonOperatorPopupDataSource::OperatorTypeForRow(selectedRow);
}

const char* HypothesisEditableController::symbolPrefix() {
  return m_test->hypothesisSymbol();
}

const HighlightCell* HypothesisEditableController::cell(int row) const {
  const HighlightCell* cells[] = {&m_h0, m_ha, &m_next};
  return cells[row];
}

bool HypothesisEditableController::hasHaDropdown() const {
  return m_test->testType() != TestType::ANOVA;
}

void HypothesisEditableController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    selectRow(0);
    if (hasHaDropdown()) {
      m_haWithDropdown.accessory()->setDropdown(&m_haDropdown);
      m_haDropdown.selectRow(
          static_cast<int>(m_test->hypothesis()->m_alternative));
      m_haDropdown.init();
      m_ha = &m_haWithDropdown;
    } else {
      m_haNoDropdown.accessory()->setText(
          Poincare::Inference::SignificanceTest::HypothesisSymbol(
              m_test->testType()));
      m_ha = &m_haNoDropdown;
    }
    loadHypothesisParam();
    App::app()->setFirstResponder(&m_selectableListView);
  } else {
    ExplicitSelectableListViewController::handleResponderChainEvent(event);
  }
}

bool HypothesisEditableController::ButtonAction(
    HypothesisEditableController* controller, void* s) {
  if (controller->m_test->testType() == TestType::ANOVA) {
    // TODO: open the next ANOVA controller
    return true;
  }
  ViewController* nextController = controller->m_inputController;
  if (controller->m_test->testType() == TestType::Slope) {
    nextController = controller->m_inputStoreController;
  } else if (controller->m_test->canChooseDataset()) {
    /* Reset row of DatasetController here and not in
     * viewWillAppear or initView because we want
     * to save row when we come back from results. */
    controller->m_datasetController->selectRow(0);
    nextController = controller->m_datasetController;
  }
  controller->stackOpenPage(nextController);
  return true;
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
