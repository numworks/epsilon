#include "hypothesis_controller.h"

#include <poincare/print.h>

#include "../controller_container.h"

using namespace Escher;

namespace Inference {

HypothesisController::HypothesisController(
    Escher::StackViewController* parent,
    ControllerContainer* controllerContainer, SignificanceTest* test)
    : Escher::ExplicitSelectableListViewController(parent),
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<HypothesisController>(
                 &HypothesisController::ButtonAction, this),
             ButtonCell::Style::EmbossedLight),
      m_test(test),
      m_controllerContainer(controllerContainer) {}

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
  ViewController* nextController =
      &controller->m_controllerContainer->m_inputController;
  if (controller->m_test->testType() == TestType::Slope) {
    nextController =
        &controller->m_controllerContainer->m_inputStoreController1;
  } else if (controller->m_test->canChooseDataset()) {
    /* Reset row of DatasetController here and not in
     * viewWillAppear or initView because we want
     * to save row when we come back from results. */
    controller->m_controllerContainer->m_datasetController.selectRow(0);
    nextController = &controller->m_controllerContainer->m_datasetController;
  }
  controller->stackOpenPage(nextController);
  return true;
}

}  // namespace Inference
