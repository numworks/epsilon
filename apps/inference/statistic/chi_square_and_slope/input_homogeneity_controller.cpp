#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, Escher::ViewController* resultsController,
    HomogeneityTest* statistic)
    : InputCategoricalController(parent, resultsController, statistic),
      m_inputHomogeneityTable(&m_selectableListView, statistic, this) {}

void InputHomogeneityController::createDynamicCells() {
  m_inputHomogeneityTable.createCells();
}

}  // namespace Inference
