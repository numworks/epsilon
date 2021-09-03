#include "input_homogeneity_data_source.h"

#include <algorithm>

#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

constexpr int InputHomogeneityDataSource::k_initialNumberOfRows;
constexpr int InputHomogeneityDataSource::k_initialNumberOfColumns;

InputHomogeneityDataSource::InputHomogeneityDataSource(
    SelectableTableView * tableView,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic,
    TextFieldDelegate * textFieldDelegate,
    DynamicTableViewDataSourceDelegate * dataSourceDelegate) :
    DynamicTableViewDataSource(dataSourceDelegate),
    m_numberOfRows(k_initialNumberOfRows),
    m_numberOfColumns(k_initialNumberOfColumns),
    m_statistic(statistic) {
  for (int i = 0; i < HomogeneityTableDataSource::k_numberOfReusableCells; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate,
                                                             textFieldDelegate);
    m_cells[i].setFont(KDFont::SmallFont);
  }
}

HighlightCell * InputHomogeneityDataSource::reusableCell(int i, int type) {
  return &m_cells[i];
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
    DynamicTableViewDataSource::notify();
  }
}

void Probability::InputHomogeneityDataSource::willDisplayCellAtLocation(
    Escher::HighlightCell * cell,
    int column,
    int row) {
  float p = m_statistic->parameterAtPosition(row, column);
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
