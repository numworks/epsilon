#include "input_anova_controller.h"

#include "inference/controllers/controller_container.h"

using namespace Escher;

namespace Inference {

InputANOVAController::InputANOVAController(
    StackViewController* parent, ControllerContainer* controllerContainer,
    ANOVATest* inference)
    : InputCategoricalController(
          parent, &controllerContainer->m_homogeneityResultsController,
          inference,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_inputANOVATable(&m_selectableListView, inference, this, this) {}

void InputANOVAController::createDynamicCells() {
  m_inputANOVATable.createCells();
}

}  // namespace Inference
