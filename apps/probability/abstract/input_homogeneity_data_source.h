#ifndef PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_DATA_SOURCE_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>

#include "probability/abstract/dynamic_cells_data_source.h"
#include "probability/abstract/dynamic_size_table_view_data_source.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/models/statistic/homogeneity_statistic.h"

namespace Probability {

using namespace Escher;

class InputHomogeneityDataSource : public Escher::TableViewDataSource, public DynamicCellsDataSource<EvenOddEditableTextCell, k_maxNumberOfEvenOddEditableTextCells>, public DynamicSizeTableViewDataSource {
public:
  InputHomogeneityDataSource(DynamicCellsDataSourceDelegate * dynamicTableViewDataSourceDelegate,
                             HomogeneityStatistic * statistic,
                             DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate);
  int numberOfRows() const override { return m_numberOfRows; }
  int numberOfColumns() const override { return m_numberOfColumns; }
  Escher::HighlightCell * reusableCell(int i, int type) override { return cell(i); }
  int reusableCellCount(int type) override {
    return HomogeneityTableDataSource::k_numberOfReusableCells;
  }
  int typeAtLocation(int i, int j) override { return 0; }

  KDCoordinate columnWidth(int i) override { return HomogeneityTableDataSource::k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return HomogeneityTableDataSource::k_rowHeight; }

  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicSizeTableViewDataSource
  bool recomputeDimensions();

private:
  constexpr static int k_initialNumberOfRows = 2;
  constexpr static int k_initialNumberOfColumns = 2;

  int m_numberOfRows;
  int m_numberOfColumns;

  HomogeneityStatistic * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_DATA_SOURCE_H */
