#include "input_slope_controller.h"
#include <poincare/print.h>

using namespace Escher;

namespace Inference {

InputSlopeController::InputSlopeController(StackViewController * parent, ViewController * resultsController, Statistic * statistic, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Poincare::Context * context) :
  InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
  m_slopeTableCell(&m_selectableTableView, this, this, statistic, context),
  m_secondStackController(this, &m_storeParameterController, StackViewController::Style::WhiteUniform),
  m_storeParameterController(parent, &m_slopeTableCell)
{
  m_storeParameterController.selectRow(0);
}

bool InputSlopeController::handleEvent(Ion::Events::Event & event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    m_storeParameterController.initializeColumnParameters();
    m_storeParameterController.selectRow(0);
    m_storeParameterController.setTitlesDisplay(ViewController::TitlesDisplay::DisplayLastTitle);
    m_secondStackController.setTitlesDisplay(titlesDisplay() == ViewController::TitlesDisplay::DisplayLastTitle ? ViewController::TitlesDisplay::DisplaySecondToLast : ViewController::TitlesDisplay::DisplaySecondAndThirdToLast);
    stackController()->push(&m_secondStackController);
    return true;
  }
  return InputCategoricalController::handleEvent(event);
}

void InputSlopeController::viewWillAppear() {
  m_slopeTableCell.willAppear();
  InputCategoricalController::viewWillAppear();
}

}
