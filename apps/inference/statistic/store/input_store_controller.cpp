#include "input_store_controller.h"

#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

InputStoreController::InputStoreController(StackViewController* parent,
                                           ViewController* resultsController,
                                           Statistic* statistic,
                                           Poincare::Context* context)
    : InputCategoricalController(parent, resultsController, statistic),
      m_dropdownCell(&m_selectableListView, &m_dropdownDataSource, this),
      m_extraParameters{
          InputCategoricalCell<LayoutView>(&m_selectableListView, this),
          InputCategoricalCell<LayoutView>(&m_selectableListView, this),
      },
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
  Table* tableModel = m_slopeTableCell.tableModel();
  if (m_statistic->significanceTestType() == SignificanceTestType::TwoMeans) {
    assert(tableModel->numberOfSeries() == 2);
    tableModel->setSeriesAt(m_statistic, 0, selectedRow / 2);
    tableModel->setSeriesAt(m_statistic, 1, ((selectedRow + 1) / 2) + 1);
  } else {
    tableModel->setSeriesAt(m_statistic, 0, selectedRow);
  }

  m_slopeTableCell.recomputeDimensionsAndReload(true);
}

KDCoordinate InputStoreController::nonMemoizedRowHeight(int row) {
  return row == indexOfTableCell() || row == indexOfSignificanceCell()
             ? InputCategoricalController::nonMemoizedRowHeight(row)
             : reusableCell(0, row)->minimalSizeForOptimalDisplay().height();
}

Escher::HighlightCell* InputStoreController::reusableCell(int index, int type) {
  assert(index == 0);
  return type == k_dropdownCellIndex ? &m_dropdownCell
         : indexOfFirstExtraParameter() <= type &&
                 type < indexOfSignificanceCell()
             ? &m_extraParameters[type - indexOfFirstExtraParameter()]
             : InputCategoricalController::reusableCell(index, type);
}

void InputStoreController::createDynamicCells() {
  m_slopeTableCell.createCells();
}

void InputStoreController::viewWillAppear() {
  for (int i = 0; i < numberOfExtraParameters(); i++) {
    InputCategoricalCell<LayoutView>& c = m_extraParameters[i];
    int param = indexOfEditedParameterAtIndex(indexOfFirstExtraParameter() + i);
    PrintValueInTextHolder(
        m_statistic->parameterAtIndex(param), c.textField(), true, true,
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits);
    c.setMessages(m_statistic->parameterSymbolAtIndex(param),
                  m_statistic->parameterDefinitionAtIndex(param));
  }

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

int InputStoreController::indexOfEditedParameterAtIndex(int index) const {
  if (index >= indexOfFirstExtraParameter() + numberOfExtraParameters()) {
    return InputCategoricalController::indexOfEditedParameterAtIndex(index);
  }
  assert(m_statistic->distributionType() == DistributionType::Z);
  return OneMean::ParamsOrder::s;
}

}  // namespace Inference
