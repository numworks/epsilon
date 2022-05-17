#include "input_slope_controller.h"
#include <poincare/print.h>

namespace Inference {

InputSlopeController::InputSlopeController(StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Table * tableModel, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_slopeTableCell(&m_selectableTableView, this, this, tableModel)
{
}

const char * InputSlopeController::title() {
  Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), I18n::translate(m_statistic->title()), I18n::translate(m_statistic->statisticBasicTitle()));
  return m_titleBuffer;
}

}
