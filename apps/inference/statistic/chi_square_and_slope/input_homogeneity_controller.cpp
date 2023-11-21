#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, Escher::ViewController* resultsController,
    HomogeneityTest* statistic)
    : InputCategoricalController(parent, resultsController, statistic),
      m_inputHomogeneityTable(&m_selectableListView, statistic, this) {}

void InputHomogeneityController::initView() {
  m_inputHomogeneityTable.createCells();
  InputCategoricalController::initView();
}

}  // namespace Inference
