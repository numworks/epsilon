#include "input_homogeneity_controller.h"

#include "inference/controllers/controller_container.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, ControllerContainer* controllerContainer,
    HomogeneityTest* inference)
    : InputCategoricalController(
          parent, &controllerContainer->m_homogeneityResultsController,
          inference,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_inputHomogeneityTable(&m_selectableListView, inference, this, this) {}

void InputHomogeneityController::createDynamicCells() {
  m_inputHomogeneityTable.createCells();
}

}  // namespace Inference
