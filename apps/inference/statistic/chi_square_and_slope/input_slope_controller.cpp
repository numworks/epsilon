#include "input_slope_controller.h"
#include <inference/models/statistic/slope_t_interval.h>
#include <inference/models/statistic/slope_t_test.h>
#include <poincare/print.h>

namespace Inference {

InputSlopeController::InputSlopeController(StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_slopeTableCell(&m_selectableTableView, this, this, tableModel())
{
}

const char * InputSlopeController::title() {
  Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), I18n::translate(m_statistic->title()), I18n::translate(m_statistic->statisticBasicTitle()));
  return m_titleBuffer;
}

Table * InputSlopeController::tableModel() {
  if (m_statistic->subApp() == Inference::SubApp::Test) {
    return static_cast<SlopeTTest *>(m_statistic);
  }
  return static_cast<SlopeTTest *>(m_statistic);
}

}
