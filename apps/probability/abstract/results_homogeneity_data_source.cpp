#include "results_homogeneity_data_source.h"

#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

ResultsHomogeneityDataSource::ResultsHomogeneityDataSource(HomogeneityStatistic * statistic) :
    m_statistic(statistic) {
  for (int i = 0; i < HomogeneityTableDataSource::k_numberOfReusableCells; i++) {
    m_cells[i].setFont(KDFont::SmallFont);
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

  constexpr int bufferSize = Constants::k_shortBufferSize;
  char buffer[bufferSize];
  defaultConvertFloatToText(value, buffer, bufferSize);
  myCell->setText(buffer);
  myCell->setEven(j % 2 == 0);
}

HomogeneityTableDataSourceWithTotals::HomogeneityTableDataSourceWithTotals(
    TableViewDataSource * contentTable,
    Escher::SelectableTableViewDelegate * tableDelegate,
    I18n::Message headerPrefix,
    I18n::Message totalMessage) :
    HomogeneityTableDataSource(contentTable, tableDelegate, headerPrefix),
    m_totalMessage(totalMessage) {
}

void HomogeneityTableDataSourceWithTotals::willDisplayCellAtLocation(Escher::HighlightCell * cell,
                                                                     int column,
                                                                     int row) {
  if ((column == 0 && row == m_innerDataSource->numberOfRows()) ||
      (row == 0 && column == m_innerDataSource->numberOfColumns())) {
    // Override to display "Total" instead
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    myCell->setText(I18n::translate(m_totalMessage));
  } else {
    HomogeneityTableDataSource::willDisplayCellAtLocation(cell, column, row);
  }
}

}  // namespace Probability
