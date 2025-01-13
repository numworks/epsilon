#include "input_store_controller.h"

#include <omg/utf8_helper.h>

#include "inference/text_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Inference {

InputStoreController::InputStoreController(
    StackViewController* parent, ViewController* nextController,
    PageIndex pageIndex, InputStoreController* nextInputStoreController,
    Statistic* statistic, Poincare::Context* context)
    : InputCategoricalController(parent, nextController, statistic),
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
      m_loadedTest(SignificanceTestType::OneProportion),
      m_pageIndex(pageIndex),
      m_nextInputStoreController(nextInputStoreController),
      m_nextOtherController(nextController) {
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

Escher::HighlightCell* InputStoreController::explicitCellAtRow(int row) {
  return row == k_dropdownCellIndex ? &m_dropdownCell
         : indexOfFirstExtraParameter() <= row &&
                 row < indexOfSignificanceCell()
             ? &m_extraParameters[row - indexOfFirstExtraParameter()]
             : InputCategoricalController::explicitCellAtRow(row);
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

  // TODO: dedicated function for the dropdown cell view initialization
  m_dropdownCell.dropdown()->init();
  Table* tableModel = m_storeTableCell.tableModel();
  if (tableModel->numberOfSeries() == 2) {
    m_dropdownCell.dropdown()->selectRow(DropdownDataSource::RowForSeriesPair(
        tableModel->seriesAt(0), tableModel->seriesAt(1)));
    if (m_pageIndex == PageIndex::One) {
      m_dropdownCell.setMessage(I18n::Message::DataSet1);
    } else {
      assert(m_pageIndex == PageIndex::Two);
      m_dropdownCell.setMessage(I18n::Message::DataSet2);
    }

  } else {
    assert(tableModel->numberOfSeries() == 1);
    m_dropdownCell.dropdown()->selectRow(tableModel->seriesAt(0));
    m_dropdownCell.setMessage(I18n::Message::DataSet);
  }
  int nRows = m_dropdownDataSource.numberOfRows();
  bool hasTwoSeries =
      m_statistic->significanceTestType() == SignificanceTestType::TwoMeans;
  constexpr size_t bufferSize =
      2 * Shared::DoublePairStore::k_tableNameLength + sizeof(",") + 1;
  char buffer[bufferSize];
  for (int row = 0; row < nRows; row++) {
    if (hasTwoSeries) {
      size_t length = store()->tableName(DropdownDataSource::Series1ForRow(row),
                                         buffer, bufferSize);
      length +=
          UTF8Helper::WriteCodePoint(buffer + length, bufferSize - length, ',');
      store()->tableName(DropdownDataSource::Series2ForRow(row),
                         buffer + length, bufferSize - length);
    } else {
      store()->tableName(row, buffer, bufferSize);
    }
    static_cast<SmallBufferTextHighlightCell*>(m_dropdownDataSource.cell(row))
        ->setText(buffer);
  }
  m_dropdownCell.dropdown()->reloadCell();

  if (hasTwoSeries)
    // TODO: refactor hideParameterCells because the page index is a member
    // variable of InputStoreController
    hideParameterCells(m_pageIndex == PageIndex::One ? 0 : 1);
  else
    setAllParameterCellsVisible();

  InputCategoricalController::viewWillAppear();
}

void InputStoreController::initView() {
  InputCategoricalController::initView();
  categoricalTableCell()->recomputeDimensions();

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

  bool shouldDisplayTwoPages =
      m_statistic->significanceTestType() == SignificanceTestType::TwoMeans;
  if (m_pageIndex == PageIndex::One) {
    // TODO: no need for setNextController because m_nextController is a class
    // member
    setNextController(shouldDisplayTwoPages ? m_nextInputStoreController
                                            : m_nextOtherController);
  }
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
