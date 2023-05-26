#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, Escher::ViewController* resultsController,
    HomogeneityTest* statistic, BoxesDelegate* boxesDelegate)
    : InputCategoricalController(parent, resultsController, statistic,
                                 boxesDelegate),
      m_inputHomogeneityTable(&m_selectableListView, statistic, this) {}

}  // namespace Inference
