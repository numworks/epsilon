#include "results_homogeneity_data_source.h"

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/text_helpers.h"

namespace Probability {

/* HomogeneityTableDataSourceWithTotals */

HomogeneityTableDataSourceWithTotals::HomogeneityTableDataSourceWithTotals(Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicDataSourceDelegate) :
  HomogeneityTableDataSource(tableDelegate, dynamicDataSourceDelegate),
  m_totalMessage(I18n::Message::Total) {
}

void HomogeneityTableDataSourceWithTotals::willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) {
  if ((column == 0 && row == innerNumberOfRows()) ||
      (row == 0 && column == innerNumberOfColumns())) {
    // Override to display "Total" instead
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    myCell->setText(I18n::translate(m_totalMessage));
  } else {
    HomogeneityTableDataSource::willDisplayCellAtLocation(cell, column, row);
  }
}

ResultsHomogeneityDataSource::ResultsHomogeneityDataSource(HomogeneityStatistic * statistic, Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicDataSourceDelegate) :
  HomogeneityTableDataSourceWithTotals(tableDelegate, dynamicDataSourceDelegate),
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>(dynamicDataSourceDelegate),
  m_statistic(statistic)
{
}

void ResultsHomogeneityDataSource::createCells() {
  if (DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::createCellsWithOffset(0);
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells * sizeof(EvenOddBufferTextCell));
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_delegate->tableView()->reloadData(false, false);
  }
}

void ResultsHomogeneityDataSource::destroyCells() {
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

void ResultsHomogeneityDataSource::willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int i, int j) {
  EvenOddBufferTextCell * myCell = static_cast<EvenOddBufferTextCell *>(cell);

  double value;
  if (i == m_statistic->numberOfResultColumns() && j == m_statistic->numberOfResultRows()) {
    value = m_statistic->total();
  } else if (i == m_statistic->numberOfResultColumns()) {
    value = m_statistic->rowTotal(j);
  } else if (j == m_statistic->numberOfResultRows()) {
    value = m_statistic->columnTotal(i);
  } else {
    value = m_statistic->expectedValueAtLocation(j, i);
  }
  // TODO: Factorize with InputCategoricalView::setTextFieldText
  constexpr int bufferSize = Constants::k_shortBufferSize;
  char buffer[bufferSize];
  defaultConvertFloatToText(value, buffer, bufferSize);
  myCell->setText(buffer);
  myCell->setEven(j % 2 == 0);
}

}  // namespace Probability
