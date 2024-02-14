#include "input_slope_controller.h"

#include <poincare/print.h>

using namespace Escher;

namespace Inference {

InputSlopeController::InputSlopeController(StackViewController* parent,
                                           ViewController* resultsController,
                                           Statistic* statistic,
                                           Poincare::Context* context)
    : InputNamedListsCategoricalController(parent, resultsController,
                                           statistic),
      m_slopeTableCell(&m_selectableListView, statistic, context, this),
      m_secondStackController(this, &m_storeParameterController,
                              StackViewController::Style::WhiteUniform),
      m_storeParameterController(parent, &m_slopeTableCell) {
  m_storeParameterController.selectRow(0);
  m_selectableListView.margins()->setTop(
      m_slopeTableCell.selectableTableView()->margins()->top());
  m_slopeTableCell.selectableTableView()->margins()->setTop(
      Metric::TableSeparatorThickness);
}

bool InputSlopeController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      selectedRow() == 0) {
    m_storeParameterController.initializeColumnParameters();
    m_storeParameterController.selectRow(0);
    m_storeParameterController.setTitlesDisplay(
        ViewController::TitlesDisplay::DisplayLastTitle);
    m_secondStackController.setTitlesDisplay(
        titlesDisplay() == ViewController::TitlesDisplay::DisplayLastTitle
            ? ViewController::TitlesDisplay::DisplaySecondToLast
            : ViewController::TitlesDisplay::DisplaySecondAndThirdToLast);
    stackController()->push(&m_secondStackController);
    return true;
  }
  return InputNamedListsCategoricalController::handleEvent(event);
}

void InputSlopeController::onDropdownSelected(int selectedRow) {
  m_slopeTableCell.store()->setSeries(selectedRow);
  m_slopeTableCell.fillColumnsNames();
  m_slopeTableCell.recomputeDimensionsAndReload(true);
}

void InputSlopeController::createDynamicCells() {
  m_slopeTableCell.createCells();
}

void InputSlopeController::viewWillAppear() {
  m_slopeTableCell.fillColumnsNames();
  InputNamedListsCategoricalController::viewWillAppear();
}

}  // namespace Inference
