#include "input_statistics_controller.h"

#include "inference/controllers/controller_container.h"

using namespace Escher;

namespace Inference::ANOVA {

InputStatisticsController::InputStatisticsController(
    StackViewController* parent, ControllerContainer* controllerContainer,
    ANOVATest* inference)
    : InputCategoricalController(
          parent, &controllerContainer->m_resultsBetweenWithinController,
          inference,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_inputANOVATable(&m_selectableListView, inference, this, this) {}

void InputStatisticsController::createDynamicCells() {
  m_inputANOVATable.createCells();
}

void InputStatisticsController::initView() {
  assert(m_inferenceModel->testType() == TestType::ANOVA);
  static_cast<ANOVATest*>(m_inferenceModel)
      ->setInputMode(ANOVATest::InputMode::Statistics);
  InputCategoricalController::initView();
}

}  // namespace Inference::ANOVA
