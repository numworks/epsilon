#include "hypothesis_controller.h"

#include <poincare/print.h>

using namespace Escher;

namespace Inference {

HypothesisController::HypothesisController(
    Escher::StackViewController* parent, InputController* inputController,
    InputStoreController* inputStoreController,
    DatasetController* datasetController, SignificanceTest* test)
    : Escher::ExplicitSelectableListViewController(parent),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<HypothesisController>(
                 &HypothesisController::ButtonAction, this),
             ButtonCell::Style::EmbossedLight),
      m_test(test),
      m_inputController(inputController),
      m_inputStoreController(inputStoreController),
      m_datasetController(datasetController) {}

const char* HypothesisController::title() const {
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(m_test->title()),
                                I18n::translate(I18n::Message::Test));
  return m_titleBuffer;
}

bool HypothesisController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

void HypothesisController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(&m_selectableListView);
  } else {
    ExplicitSelectableListViewController::handleResponderChainEvent(event);
  }
}

bool HypothesisController::ButtonAction(HypothesisController* controller,
                                        void* s) {
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
