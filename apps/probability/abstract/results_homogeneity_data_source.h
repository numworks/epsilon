#ifndef PROBABILITY_ABSTRACT_RESULTS_HOMOGENEITY_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_RESULTS_HOMOGENEITY_DATA_SOURCE_H

#include "homogeneity_data_source.h"

namespace Probability {

/* This class adds a row / column header to the ResultsHomogeneityDataSource. */
class HomogeneityTableDataSourceWithTotals : public HomogeneityTableDataSource {
public:
  HomogeneityTableDataSourceWithTotals(Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicDataSourceDelegate);
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

private:
  I18n::Message m_totalMessage;
};

/* This DataSource reads the statistic and displays the expected values and the totals for each cell
 * position. */
class ResultsHomogeneityDataSource : public HomogeneityTableDataSourceWithTotals, DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells> {
public:
  ResultsHomogeneityDataSource(HomogeneityTest * statistic, Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicDataSourceDelegate);
private:
  int innerNumberOfRows() const override { return m_statistic->numberOfResultRows() + 1; }
  int innerNumberOfColumns() const override { return m_statistic->numberOfResultColumns() + 1; }
  Escher::HighlightCell * innerCell(int i) override { return DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::cell(i); }
  void willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  // DynamicCellsDataSource
  void createCells() override;
  void destroyCells() override;

  HomogeneityTest * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_RESULTS_HOMOGENEITY_DATA_SOURCE_H */
