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
    : InputCategoricalController(
          parent, nextController, statistic,
          Invocation::Builder<InputStoreController>(
              &InputStoreController::ButtonAction, this)),
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

const Escher::HighlightCell* InputStoreController::explicitCellAtRow(
    int row) const {
  if (row == k_dropdownCellIndex) {
    return &m_dropdownCell;
  }
  if (row == indexOfTableCell()) {
    return &m_storeTableCell;
  }
  if (indexOfFirstExtraParameter() <= row && row < indexOfSignificanceCell()) {
    return &m_extraParameters[row - indexOfFirstExtraParameter()];
  }
  return InputCategoricalController::explicitCellAtRow(row);
}

HighlightCell* InputStoreController::explicitCellAtRow(int row) {
  return const_cast<HighlightCell*>(
      const_cast<const InputStoreController*>(this)->explicitCellAtRow(row));
}

void InputStoreController::createDynamicCells() {
  m_storeTableCell.createCells();
}

void InputStoreController::initializeDropdown() {
  m_dropdownCell.dropdown()->init();
  const RawDataStatistic* model =
      static_cast<const RawDataStatistic*>(m_storeTableCell.tableModel());

  if (shouldDisplayTwoPages()) {
    if (m_pageIndex == PageIndex::One) {
      m_dropdownCell.setMessage(I18n::Message::DataSet1);
    } else {
      assert(m_pageIndex == PageIndex::Two);
      m_dropdownCell.setMessage(I18n::Message::DataSet2);
    }
  } else {
    assert(model->numberOfSeries() == 1);
    m_dropdownCell.setMessage(I18n::Message::DataSet);
  }
  m_dropdownCell.dropdown()->selectRow(
      model->seriesAt(static_cast<uint8_t>(m_pageIndex)));

  int nRows = m_dropdownDataSource.numberOfRows();
  constexpr size_t bufferSize =
      2 * Shared::DoublePairStore::k_tableNameLength + sizeof(",") + 1;
  char buffer[bufferSize];
  for (int row = 0; row < nRows; row++) {
    store()->tableName(row, buffer, bufferSize);
    static_cast<SmallBufferTextHighlightCell*>(m_dropdownDataSource.cell(row))
        ->setText(buffer);
  }
  m_dropdownCell.dropdown()->reloadCell();
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

  initializeDropdown();

  InputCategoricalController::viewWillAppear();

  m_selectableListView.layoutSubviews(true);
}

void InputStoreController::initView() {
  InputCategoricalController::initView();
  static_cast<RawDataStatistic*>(m_storeTableCell.tableModel())
      ->setActivePage(m_pageIndex);
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
  if (m_pageIndex == PageIndex::One) {
    m_nextController = shouldDisplayTwoPages() ? m_nextInputStoreController
                                               : m_nextOtherController;
  }
  updateParameterCellsVisibility();
}

KDCoordinate InputStoreController::separatorBeforeRow(int row) const {
  /* A separator before the last cell (the "next" button) must be added, with
   * an exception: no such separator should be added if all cells between the
   * "table" cell and the "next" cell are set to invisible, otherwise an
   * unwanted thin white line appears. */
  if (row == indexOfNextCell() && !areAllParameterCellsInvisible()) {
    return k_defaultRowSeparator;
  }
  return 0;
}

bool InputStoreController::ButtonAction(InputStoreController* controller,
                                        void* s) {
  if (controller->m_nextInputStoreController == controller->m_nextController) {
    controller->m_nextInputStoreController->initSeriesSelection();
  }
  return InputCategoricalController::ButtonAction(controller, s);
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
  tableModel->setSeriesAt(m_statistic, static_cast<uint8_t>(m_pageIndex), row);
}

void InputStoreController::updateParameterCellsVisibility() {
  if (!shouldDisplayTwoPages()) {
    setAllParameterCellsVisible();
  } else {
    hideOtherPageParameterCells();
  }
}

void InputStoreController::hideOtherPageParameterCells() {
  assert(shouldDisplayTwoPages());
  if (m_pageIndex == PageIndex::One) {
    // The significance cell is visible only on the second page
    m_significanceCell.setVisible(false);
  }

  if (m_statistic->distributionType() == DistributionType::Z) {
    assert(numberOfExtraParameters() == 2);
    // Hide the parameter of the other dataset
    m_extraParameters[(static_cast<uint8_t>(m_pageIndex) + 1) % 2].setVisible(
        false);
  } else {
    /* In the TwoMeans test, there are either 2 extra parameters (for the Z
     * distribution type), or 0 extra parameter (for the T distribution type)
     */
    assert(numberOfExtraParameters() == 0);
  }
}

void InputStoreController::setAllParameterCellsVisible() {
  assert(!shouldDisplayTwoPages());
  std::for_each(m_extraParameters,
                m_extraParameters + numberOfExtraParameters(),
                [](InputCategoricalCell<Escher::LayoutView>& cell) {
                  cell.setVisible(true);
                });
  m_significanceCell.setVisible(true);
}

bool InputStoreController::areAllParameterCellsInvisible() const {
  for (int row = indexOfTableCell() + 1; row < indexOfNextCell(); row++) {
    if (explicitCellAtRow(row)->isVisible()) {
      return false;
    }
  }
  return true;
}

}  // namespace Inference
