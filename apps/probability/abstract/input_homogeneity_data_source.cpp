#include "input_homogeneity_data_source.h"

#include <algorithm>

#include "probability/text_helpers.h"

namespace Probability {

InputHomogeneityDataSource::InputHomogeneityDataSource(
    SelectableTableView * tableView,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic,
    TextFieldDelegate * textFieldDelegate,
    DynamicTableViewDataSourceDelegate * dataSourceDelegate) :
    DynamicTableViewDataSource(dataSourceDelegate),
    m_numberOfRows(HomogeneityTableDataSource::k_initialNumberOfRows),
    m_numberOfColumns(HomogeneityTableDataSource::k_initialNumberOfColumns),
    m_statistic(statistic),
    m_table(tableView) {
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
  m_numberOfRows = std::max(HomogeneityTableDataSource::k_initialNumberOfRows, dimensions.row + 1);
  m_numberOfColumns = std::max(HomogeneityTableDataSource::k_initialNumberOfColumns,
                               dimensions.col + 1);
  DynamicTableViewDataSource::notify();
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
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    defaultParseFloat(p, buffer, bufferSize);
    myCell->editableTextCell()->textField()->setText(buffer);
  }
  myCell->setEven(row % 2 == 0);
}

}  // namespace Probability
