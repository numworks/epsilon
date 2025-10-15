#include "hypothesis_display_only_controller.h"

#include <poincare/print.h>

using namespace Escher;

namespace Inference {

HypothesisDisplayOnlyController::HypothesisDisplayOnlyController(
    Escher::StackViewController* parent, InputController* inputController,
    InputStoreController* inputStoreController,
    DatasetController* datasetController, SignificanceTest* test)
    : Escher::ExplicitSelectableListViewController(parent),
      m_inputController(inputController),
      m_inputStoreController(inputStoreController),
      m_datasetController(datasetController),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<HypothesisDisplayOnlyController>(
                 &HypothesisDisplayOnlyController::ButtonAction, this),
             ButtonCell::Style::EmbossedLight),
      m_test(test) {
  m_h0.label()->setLayout("H"_l ^ KSubscriptL("0"_l));
  m_h0.subLabel()->setMessage(I18n::Message::H0Sub);
  m_h0.accessory()->setText(
      Poincare::Inference::SignificanceTest::HypothesisSymbol(
          m_test->testType()));
  m_ha.label()->setLayout("H"_l ^ KSubscriptL("a"_l));
  m_ha.subLabel()->setMessage(I18n::Message::HaSub);
  m_ha.accessory()->setText(
      Poincare::Inference::SignificanceTest::HypothesisSymbol(
          m_test->testType()));
}

const char* HypothesisDisplayOnlyController::title() const {
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(m_test->title()),
                                I18n::translate(I18n::Message::Test));
  return m_titleBuffer;
}

bool HypothesisDisplayOnlyController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

const HighlightCell* HypothesisDisplayOnlyController::cell(int row) const {
  const HighlightCell* cells[] = {&m_h0, &m_ha, &m_next};
  return cells[row];
}

void HypothesisDisplayOnlyController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    selectRow(0);
    App::app()->setFirstResponder(&m_selectableListView);
  } else {
    ExplicitSelectableListViewController::handleResponderChainEvent(event);
  }
}

bool HypothesisDisplayOnlyController::ButtonAction(
    HypothesisDisplayOnlyController* controller, void* s) {
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

}  // namespace Inference
