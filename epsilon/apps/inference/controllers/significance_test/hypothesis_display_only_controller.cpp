#include "hypothesis_display_only_controller.h"

namespace Inference {

HypothesisDisplayOnlyController::HypothesisDisplayOnlyController(
    Escher::StackViewController* parent, InputController* inputController,
    InputStoreController* inputStoreController,
    DatasetController* datasetController, SignificanceTest* test)
    : HypothesisController(parent, inputController, inputStoreController,
                           datasetController, test) {
  m_h0.label()->setLayout("H"_l ^ KSubscriptL("0"_l));
  m_h0.subLabel()->setMessage(I18n::Message::H0Sub);
  m_ha.label()->setLayout("H"_l ^ KSubscriptL("a"_l));
  m_ha.subLabel()->setMessage(I18n::Message::HaSub);
}

const Escher::HighlightCell* HypothesisDisplayOnlyController::cell(
    int row) const {
  const Escher::HighlightCell* cells[] = {&m_h0, &m_ha, &m_next};
  return cells[row];
}

void HypothesisDisplayOnlyController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    selectRow(0);
    /* NOTE: the class instance is constructed when the Inference app is opened.
     * This means that m_test->testType() does not point to a specific test type
     * in the constructor. Data that depends on the test type has to be updated
     * in functions that are called when the controller becomes active. */
    m_h0.accessory()->setText(
        Poincare::Inference::SignificanceTest::HypothesisSymbol(
            m_test->testType()));
    m_ha.accessory()->setText(
        Poincare::Inference::SignificanceTest::HypothesisSymbol(
            m_test->testType()));
    m_selectableListView.reloadData();
  }
  HypothesisController::handleResponderChainEvent(event);
}

}  // namespace Inference
