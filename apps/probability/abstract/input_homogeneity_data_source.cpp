#include "input_homogeneity_data_source.h"

#include <algorithm>

#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

constexpr int InputHomogeneityDataSource::k_initialNumberOfRows;
constexpr int InputHomogeneityDataSource::k_initialNumberOfColumns;

InputHomogeneityDataSource::InputHomogeneityDataSource(Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicOuterTableViewDataSourceDelegate, DynamicCellsDataSourceDelegate<EvenOddEditableTextCell> * dynamicInnerTableViewDataSourceDelegate, DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate, HomogeneityStatistic * statistic) :
  HomogeneityTableDataSource(tableDelegate, dynamicOuterTableViewDataSourceDelegate),
  DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>(dynamicInnerTableViewDataSourceDelegate),
  DynamicSizeTableViewDataSource(dataSourceDelegate),
  m_numberOfRows(k_initialNumberOfRows),
  m_numberOfColumns(k_initialNumberOfColumns),
  m_statistic(statistic) {
}

void InputHomogeneityDataSource::createCells() {
  // We could equivalently use DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::m_cells
  if (DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::createCellsWithOffset(0);
    DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells * sizeof(EvenOddBufferTextCell));
    // We could equivalently use DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::m_delegate
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_delegate->tableView()->reloadData(false, false);
  }
}

void InputHomogeneityDataSource::destroyCells() {
  DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

// TODO: factorize with InputGoodnessTableView
bool InputHomogeneityDataSource::recomputeDimensions() {
  // Return true if size changed
  HomogeneityStatistic::Index2D dimensions = m_statistic->computeDimensions();
  bool displayLastEmptyRow = dimensions.row < m_statistic->maxNumberOfRows();
  bool displayLastEmptyColumn = dimensions.col < m_statistic->maxNumberOfColumns();
  int newNumberOfRows = std::max(k_initialNumberOfRows, dimensions.row + displayLastEmptyRow);
  int newNumberOfColumns = std::max(k_initialNumberOfColumns,
                                    dimensions.col + displayLastEmptyColumn);
  if (newNumberOfRows != m_numberOfRows || newNumberOfColumns != m_numberOfColumns) {
    m_numberOfRows = newNumberOfRows;
    m_numberOfColumns = newNumberOfColumns;
    DynamicSizeTableViewDataSource::didChangeSize();
    return true;
  }
  return false;
}

void Probability::InputHomogeneityDataSource::willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) {
  double p = m_statistic->parameterAtPosition(row, column);
  Escher::EvenOddEditableTextCell * myCell = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  if (std::isnan(p)) {
    myCell->editableTextCell()->textField()->setText("");
  } else {
    // TODO: Factorize with InputCategoricalView::setTextFieldText
    constexpr int bufferSize = Constants::k_shortBufferSize;
    char buffer[bufferSize];
    defaultConvertFloatToText(p, buffer, bufferSize);
    myCell->editableTextCell()->textField()->setText(buffer);
  }
  myCell->setEven(row % 2 == 0);
}

}  // namespace Probability
