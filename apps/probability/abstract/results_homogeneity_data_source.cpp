#include "results_homogeneity_data_source.h"

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

ResultsHomogeneityDataSource::ResultsHomogeneityDataSource(HomogeneityStatistic * statistic) :
      m_statistic(statistic),
      m_cells(nullptr)
{
}

void ResultsHomogeneityDataSource::createCells() {
  if (m_cells == nullptr) {
    static_assert(sizeof(EvenOddBufferTextCell) * HomogeneityTableDataSource::k_numberOfReusableCells <= Probability::App::k_bufferSize, "Probability::App::m_buffer is not large enough");
    m_cells = new (Probability::App::app()->buffer()) EvenOddBufferTextCell[HomogeneityTableDataSource::k_numberOfReusableCells];
    Probability::App::app()->setBufferDestructor(ResultsHomogeneityDataSource::destroyCells);
    for (int i = 0; i < HomogeneityTableDataSource::k_numberOfReusableCells; i++) {
      m_cells[i].setFont(KDFont::SmallFont);
    }
  }
}

void ResultsHomogeneityDataSource::destroyCells(void * cells) {
  delete [] static_cast<EvenOddBufferTextCell *>(cells);
}

HighlightCell * ResultsHomogeneityDataSource::reusableCell(int i, int type) {
  createCells();
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
