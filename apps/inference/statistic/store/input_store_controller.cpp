#include "input_store_controller.h"

using namespace Escher;

namespace Inference {

InputStoreController::InputStoreController(StackViewController* parent,
                                           ViewController* resultsController,
                                           Statistic* statistic,
                                           Poincare::Context* context)
    : InputCategoricalController(parent, resultsController, statistic),
      m_dropdownCell(&m_selectableListView, &m_dropdownDataSource, this),
      m_slopeTableCell(&m_selectableListView, statistic, context, this),
      m_secondStackController(this, &m_storeParameterController,
                              StackViewController::Style::WhiteUniform),
      m_storeParameterController(parent, &m_slopeTableCell) {
  m_dropdownCell.setMessage(I18n::Message::DataSet);
  m_storeParameterController.selectRow(0);
  m_selectableListView.margins()->setTop(
      m_slopeTableCell.selectableTableView()->margins()->top());
  m_slopeTableCell.selectableTableView()->margins()->setTop(
      Metric::TableSeparatorThickness);
}

bool InputStoreController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      selectedRow() == indexOfTableCell()) {
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
  return InputCategoricalController::handleEvent(event);
}

void InputStoreController::onDropdownSelected(int selectedRow) {
  m_slopeTableCell.setSelectedSeries(selectedRow);
  m_slopeTableCell.fillColumnsNames();
  m_slopeTableCell.recomputeDimensionsAndReload(true);
}

void InputStoreController::createDynamicCells() {
  m_slopeTableCell.createCells();
}

void InputStoreController::viewWillAppear() {
  m_slopeTableCell.fillColumnsNames();
  m_dropdownCell.dropdown()->init();
  for (int row = 0; row < m_dropdownDataSource.numberOfRows(); row++) {
    char index = '1' + row;
    char buffer[] = {listPrefix(0), index, '/', listPrefix(1), index, '\0'};
    static_cast<SmallBufferTextHighlightCell*>(m_dropdownDataSource.cell(row))
        ->setText(buffer);
  }
  m_dropdownCell.dropdown()->reloadCell();
  InputCategoricalController::viewWillAppear();
}

}  // namespace Inference
