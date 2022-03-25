#include "input_homogeneity_controller.h"

namespace Probability {

InputHomogeneityController::InputHomogeneityController(StackViewController * parent, Escher::ViewController * resultsController, HomogeneityTest * statistic, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_inputHomogeneityTable(&m_selectableTableView, this, this, statistic)
{}

}
