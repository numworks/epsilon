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
      m_storeTableCell(&m_selectableListView, statistic, context, this, this),
      m_secondStackController(this, &m_storeParameterController,
                              StackViewController::Style::WhiteUniform),
      m_storeParameterController(parent, &m_storeTableCell),
      m_loadedSubApp(Statistic::SubApp::Test),
      m_loadedDistribution(DistributionType::T),
      m_loadedTest(SignificanceTestType::OneProportion) {
  m_storeParameterController.selectRow(0);
  m_selectableListView.margins()->setTop(Metric::CommonMargins.top());
  m_storeTableCell.selectableTableView()->margins()->setTop(
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
        TitlesDisplay(static_cast<int>(titlesDisplay()) << 1));
    stackController()->push(&m_secondStackController);
    return true;
  }
  return InputCategoricalController::handleEvent(event);
}

void InputStoreController::onDropdownSelected(int selectedRow) {
  selectSeriesForDropdownRow(selectedRow);
  m_storeTableCell.recomputeDimensionsAndReload(true);
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
  m_storeTableCell.createCells();
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
  Table* tableModel = m_storeTableCell.tableModel();
  if (tableModel->numberOfSeries() == 2) {
    m_dropdownCell.dropdown()->selectRow(DropdownDataSource::RowForSeriesPair(
        tableModel->seriesAt(0), tableModel->seriesAt(1)));
    m_dropdownCell.setMessage(I18n::Message::DataSets);
  } else {
    assert(tableModel->numberOfSeries() == 1);
    m_dropdownCell.dropdown()->selectRow(tableModel->seriesAt(0));
    m_dropdownCell.setMessage(I18n::Message::DataSet);
  }
  for (int row = 0; row < m_dropdownDataSource.numberOfRows(); row++) {
    char buffer[] = "Ai/Bi,Aj/Bj";
    buffer[0] = listPrefix(0);
    buffer[3] = listPrefix(1);
    if (m_statistic->significanceTestType() == SignificanceTestType::TwoMeans) {
      char i = '1' + DropdownDataSource::Series1ForRow(row);
      char j = '1' + DropdownDataSource::Series2ForRow(row);
      buffer[6] = listPrefix(0);
      buffer[9] = listPrefix(1);
      buffer[1] = buffer[4] = i;
      buffer[7] = buffer[10] = j;
    } else {
      char i = '1' + row;
      buffer[1] = buffer[4] = i;
      buffer[5] = '\0';
    }
    static_cast<SmallBufferTextHighlightCell*>(m_dropdownDataSource.cell(row))
        ->setText(buffer);
  }
  m_dropdownCell.dropdown()->reloadCell();

  if (m_loadedSubApp != m_statistic->subApp() ||
      m_loadedDistribution != m_statistic->distributionType() ||
      m_loadedTest != m_statistic->significanceTestType()) {
    categoricalTableCell()->selectRow(-1);
    categoricalTableCell()->selectColumn(0);
    categoricalTableCell()->selectableTableView()->resetScroll();
    m_selectableListView.selectRow(0);
    m_selectableListView.resetScroll();
  }
  m_loadedSubApp = m_statistic->subApp();
  m_loadedDistribution = m_statistic->distributionType();
  m_loadedTest = m_statistic->significanceTestType();

  InputCategoricalController::viewWillAppear();
}

int InputStoreController::indexOfEditedParameterAtIndex(int index) const {
  if (index >= indexOfFirstExtraParameter() + numberOfExtraParameters()) {
    return InputCategoricalController::indexOfEditedParameterAtIndex(index);
  }
  assert(m_statistic->distributionType() == DistributionType::Z);
  if (m_statistic->significanceTestType() == SignificanceTestType::OneMean) {
    assert(index == indexOfFirstExtraParameter());
    return OneMean::ParamsOrder::s;
  }
  assert(m_statistic->significanceTestType() == SignificanceTestType::TwoMeans);
  return index == indexOfFirstExtraParameter() ? TwoMeans::ParamsOrder::s1
                                               : TwoMeans::ParamsOrder::s2;
}

void InputStoreController::selectSeriesForDropdownRow(int row) {
  if (row < 0) {
    row = 0;
  }
  Table* tableModel = m_storeTableCell.tableModel();
  if (m_statistic->significanceTestType() == SignificanceTestType::TwoMeans) {
    assert(tableModel->numberOfSeries() == 2);
    tableModel->setSeriesAt(m_statistic, 0,
                            DropdownDataSource::Series1ForRow(row));
    tableModel->setSeriesAt(m_statistic, 1,
                            DropdownDataSource::Series2ForRow(row));
  } else {
    tableModel->setSeriesAt(m_statistic, 0, row);
  }
}

}  // namespace Inference
