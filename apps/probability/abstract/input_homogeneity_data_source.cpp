#include "input_homogeneity_data_source.h"

#include "probability/text_helpers.h"

namespace Probability {

InputHomogeneityDataSource::InputHomogeneityDataSource(
    SelectableTableView * tableView,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    m_numberOfRows(HomogeneityTableDataSource::k_initialNumberOfRows),
    m_numberOfColumns(HomogeneityTableDataSource::k_initialNumberOfColumns),
    m_statistic(statistic),
    m_table(tableView) {
  for (int i = 0; i < HomogeneityTableDataSource::k_maxNumberOfInnerCells; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    m_cells[i].setFont(KDFont::SmallFont);
  }
}

HighlightCell * InputHomogeneityDataSource::reusableCell(int i, int type) {
  return &m_cells[i];
}

bool Probability::InputHomogeneityDataSource::textFieldShouldFinishEditing(
    Escher::TextField * textField,
    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool Probability::InputHomogeneityDataSource::textFieldDidFinishEditing(
    Escher::TextField * textField,
    const char * text,
    Ion::Events::Event event) {
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }

  m_statistic->setParameterAtPosition(m_table->selectedRow() - 1, m_table->selectedColumn() - 1, p);
  if (m_table->selectedRow() == numberOfRows() &&
      numberOfRows() < HomogeneityTableDataSource::k_maxNumberOfRows) {
    m_numberOfRows++;
  }
  if (m_table->selectedColumn() == numberOfColumns() &&
      numberOfColumns() < HomogeneityTableDataSource::k_maxNumberOfColumns) {
    m_numberOfColumns++;
  }
  m_table->selectCellAtLocation(m_table->selectedColumn(), m_table->selectedRow() + 1);
  m_table->reloadData(false);
  return true;
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
