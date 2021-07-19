#include "results_homogeneity_data_source.h"

#include "probability/text_helpers.h"

namespace Probability {

ResultsHomogeneityDataSource::ResultsHomogeneityDataSource(HomogeneityStatistic * statistic) :
    m_statistic(statistic) {
  for (int i = 0; i < HomogeneityTableDataSource::k_numberOfReusableCells; i++) {
    m_cells[i].setFont(KDFont::SmallFont);
    m_cells[i].setText("1");
    m_cells[i].setEven(i % HomogeneityTableDataSource::k_initialNumberOfRows == 0);
  }
}

HighlightCell * ResultsHomogeneityDataSource::reusableCell(int i, int type) {
  return &m_cells[i];
}

void ResultsHomogeneityDataSource::willDisplayCellAtLocation(Escher::HighlightCell * cell,
                                                             int i,
                                                             int j) {
  EvenOddBufferTextCell * myCell = static_cast<EvenOddBufferTextCell *>(cell);

  float value;
  if (i == m_statistic->numberOfResultColumns() && j == m_statistic->numberOfResultRows()) {
    value = m_statistic->total();
  } else if (i == m_statistic->numberOfResultColumns()) {
    value = m_statistic->rowTotal(j);
  } else if (j == m_statistic->numberOfResultRows()) {
    value = m_statistic->columnTotal(i);
  } else {
    value = m_statistic->expectedValueAtLocation(j, i);
  }

  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  defaultParseFloat(value, buffer, bufferSize);
  myCell->setText(buffer);
  myCell->setEven(j % 2 == 0);
}

HomogeneityTableDataSourceWithTotals::HomogeneityTableDataSourceWithTotals(
    TableViewDataSource * contentTable,
    I18n::Message headerPrefix,
    I18n::Message totalMessage) :
    HomogeneityTableDataSource(contentTable, headerPrefix), m_totalMessage(totalMessage) {
}

void HomogeneityTableDataSourceWithTotals::willDisplayCellAtLocation(Escher::HighlightCell * cell,
                                                                     int column,
                                                                     int row) {
  if ((column == 0 && row == m_contentTable->numberOfRows()) ||
      (row == 0 && column == m_contentTable->numberOfColumns())) {
    // Override to display "Total" instead
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    myCell->setText(I18n::translate(m_totalMessage));
  } else {
    HomogeneityTableDataSource::willDisplayCellAtLocation(cell, column, row);
  }
}

}  // namespace Probability
