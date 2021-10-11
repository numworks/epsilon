#include "input_homogeneity_data_source.h"

#include <algorithm>

#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

constexpr int InputHomogeneityDataSource::k_initialNumberOfRows;
constexpr int InputHomogeneityDataSource::k_initialNumberOfColumns;

InputHomogeneityDataSource::InputHomogeneityDataSource(
    DynamicCellsDataSourceDelegate * dynamicTableViewDataSourceDelegate,
    HomogeneityStatistic * statistic,
    DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate) :
      DynamicCellsDataSource<EvenOddEditableTextCell, k_maxNumberOfEvenOddEditableTextCells>(dynamicTableViewDataSourceDelegate),
      DynamicSizeTableViewDataSource(dataSourceDelegate),
      m_numberOfRows(k_initialNumberOfRows),
      m_numberOfColumns(k_initialNumberOfColumns),
      m_statistic(statistic) {
}

void InputHomogeneityDataSource::recomputeDimensions() {
  HomogeneityStatistic::Index2D dimensions = m_statistic->computeDimensions();
  bool displayLastEmptyRow = dimensions.row < HomogeneityStatistic::k_maxNumberOfRows;
  bool displayLastEmptyColumn = dimensions.col < HomogeneityStatistic::k_maxNumberOfColumns;
  int newNumberOfRows = std::max(k_initialNumberOfRows, dimensions.row + displayLastEmptyRow);
  int newNumberOfColumns = std::max(k_initialNumberOfColumns,
                                    dimensions.col + displayLastEmptyColumn);
  if (newNumberOfRows != m_numberOfRows || newNumberOfColumns != m_numberOfColumns) {
    m_numberOfRows = newNumberOfRows;
    m_numberOfColumns = newNumberOfColumns;
    DynamicSizeTableViewDataSource::didChangeSize();
  }
}

void Probability::InputHomogeneityDataSource::willDisplayCellAtLocation(
    Escher::HighlightCell * cell,
    int column,
    int row) {
  double p = m_statistic->parameterAtPosition(row, column);
  Escher::EvenOddEditableTextCell * myCell = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  if (std::isnan(p)) {
    myCell->editableTextCell()->textField()->setText("");
  } else {
    constexpr int bufferSize = Constants::k_shortBufferSize;
    char buffer[bufferSize];
    defaultConvertFloatToText(p, buffer, bufferSize);
    myCell->editableTextCell()->textField()->setText(buffer);
  }
  myCell->setEven(row % 2 == 0);
}

}  // namespace Probability
