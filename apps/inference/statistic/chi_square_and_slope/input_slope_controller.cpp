#include "input_slope_controller.h"
#include <poincare/print.h>

namespace Inference {

InputSlopeController::InputSlopeController(StackViewController * parent, Escher::ViewController * resultsController, Statistic * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Poincare::Context * context) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_slopeTableCell(&m_selectableTableView, this, this, statistic, context),
  m_storeParameterController(nullptr, &m_slopeTableCell)
{
}

bool InputSlopeController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    m_storeParameterController.initializeColumnParameters();
    m_storeParameterController.selectRow(0);
    m_storeParameterController.setTitlesDisplay(titlesDisplay() == ViewController::TitlesDisplay::DisplayLastTitle ? ViewController::TitlesDisplay::DisplayLastTwoTitles : ViewController::TitlesDisplay::DisplayLastThreeTitles);
    stackController()->push(&m_storeParameterController);
    return true;
  }
  return InputCategoricalController::handleEvent(event);
}

}
