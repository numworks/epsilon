#include "input_data_anova_controller.h"

#include "inference/controllers/controller_container.h"

using namespace Escher;

namespace Inference {

InputDataANOVAController::InputDataANOVAController(
    StackViewController* parent, ControllerContainer* controllerContainer,
    ANOVATest* inference)
    : InputCategoricalController(
          parent, &controllerContainer->m_resultsANOVAController, inference,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_inputANOVATable(&m_selectableListView, inference, this, this) {}

void InputDataANOVAController::createDynamicCells() {
  m_inputANOVATable.createCells();
}

}  // namespace Inference
