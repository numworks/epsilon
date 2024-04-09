#include "hypothesis_controller.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <escher/invocation.h>
#include <escher/stack_view_controller.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/print.h>
#include <shared/poincare_helpers.h>
#include <string.h>

#include "inference/app.h"
#include "inference/statistic/input_controller.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

HypothesisController::HypothesisController(
    Escher::StackViewController* parent, InputController* inputController,
    InputStoreController* inputStoreController,
    DatasetController* datasetController, Test* test)
    : Escher::ExplicitSelectableListViewController(parent),
      m_inputController(inputController),
      m_inputStoreController(inputStoreController),
      m_datasetController(datasetController),
      m_operatorDataSource(test),
      m_h0(&m_selectableListView, this),
      m_haDropdown(&m_selectableListView, &m_operatorDataSource, this),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<HypothesisController>(
                 &HypothesisController::ButtonAction, this),
             ButtonCell::Style::EmbossedLight),
      m_test(test) {
  m_h0.label()->setLayout("H"_l ^ KSubscriptL("0"_l));
  m_h0.subLabel()->setMessage(I18n::Message::H0Sub);
  m_ha.label()->setLayout("H"_l ^ KSubscriptL("a"_l));
  m_ha.subLabel()->setMessage(I18n::Message::HaSub);
  m_ha.accessory()->setDropdown(&m_haDropdown);
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

bool HypothesisController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  double h0 =
      Poincare::Expression::ParseAndSimplifyAndApproximateToScalar<double>(
          textField->draftText(),
          AppsContainerHelper::sharedAppsContainerGlobalContext());
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

void HypothesisController::textFieldDidAbortEditing(
    AbstractTextField* textField) {
  // Reload params to add "p=..."
  loadHypothesisParam();
}

void HypothesisController::onDropdownSelected(int selectedRow) {
  m_test->hypothesisParams()->setComparisonOperator(
      ComparisonOperatorPopupDataSource::OperatorTypeForRow(selectedRow));
}

const char* HypothesisController::symbolPrefix() {
  return m_test->hypothesisSymbol();
}

HighlightCell* HypothesisController::cell(int row) {
  HighlightCell* cells[] = {&m_h0, &m_ha, &m_next};
  return cells[row];
}

void HypothesisController::didBecomeFirstResponder() {
  selectRow(0);
  m_h0.setEditable(m_test->significanceTestType() !=
                   SignificanceTestType::Slope);
  m_haDropdown.selectRow(
      static_cast<int>(m_test->hypothesisParams()->comparisonOperator()));
  m_haDropdown.init();
  loadHypothesisParam();
  App::app()->setFirstResponder(&m_selectableListView);
}

bool HypothesisController::ButtonAction(HypothesisController* controller,
                                        void* s) {
  ViewController* nextController = controller->m_inputController;
  if (controller->m_test->significanceTestType() ==
      SignificanceTestType::Slope) {
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

void HypothesisController::loadHypothesisParam() {
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed", symbolPrefix(),
      m_test->hypothesisParams()->firstParam(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits);
  m_h0.textField()->setText(buffer);
  m_operatorDataSource.updateMessages();
  m_haDropdown.reloadCell();
  m_selectableListView.reloadData();
}

}  // namespace Inference
