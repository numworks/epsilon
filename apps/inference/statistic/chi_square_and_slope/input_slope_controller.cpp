#include "input_slope_controller.h"
#include <poincare/print.h>

namespace Inference {

InputSlopeController::InputSlopeController(StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Poincare::Context * context) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_slopeTableCell(&m_selectableTableView, this, this, statistic, context)
{
}

}
