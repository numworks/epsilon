#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityController::InputHomogeneityController(
    StackViewController* parent, Escher::ViewController* resultsController,
    HomogeneityTest* statistic,
    InputEventHandlerDelegate* inputEventHandlerDelegate)
    : InputCategoricalController(parent, resultsController, statistic,
                                 inputEventHandlerDelegate),
      m_inputHomogeneityTable(&m_selectableListView, statistic, this) {}

}  // namespace Inference
