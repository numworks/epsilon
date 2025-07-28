#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, Escher::ViewController* resultsController,
    HomogeneityTest* inference)
    : InputCategoricalController(
          parent, resultsController, inference,
          Invocation::Builder<InputCategoricalController>(
              &InputCategoricalController::ButtonAction, this)),
      m_inputHomogeneityTable(&m_selectableListView, inference, this, this) {}

void InputHomogeneityController::createDynamicCells() {
  m_inputHomogeneityTable.createCells();
}

}  // namespace Inference
