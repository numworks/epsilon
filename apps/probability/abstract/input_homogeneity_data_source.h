#ifndef PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_DATA_SOURCE_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>

#include "probability/abstract/dynamic_cells_data_source.h"
#include "probability/abstract/dynamic_size_table_view_data_source.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/models/statistic/homogeneity_test.h"

namespace Probability {

using namespace Escher;

class InputHomogeneityDataSource : public HomogeneityTableDataSource, public DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>, public DynamicSizeTableViewDataSource {
public:
  InputHomogeneityDataSource(Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicOuterTableViewDataSourceDelegate, DynamicCellsDataSourceDelegate<EvenOddEditableTextCell> * dynamicInnerTableViewDataSourceDelegate, DynamicSizeTableViewDataSourceDelegate * dataSourceDelegate, HomogeneityTest * statistic);

  // DynamicSizeTableViewDataSource
  bool recomputeDimensions();

  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }
private:
  constexpr static int k_initialNumberOfRows = 2;
  constexpr static int k_initialNumberOfColumns = 2;

  Escher::HighlightCell * innerCell(int i) override { return DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::cell(i); }
  void willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicCellsDataSource
  void createCells() override;
  void destroyCells() override;

  int m_numberOfRows;
  int m_numberOfColumns;

  HomogeneityTest * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_INPUT_HOMOGENEITY_DATA_SOURCE_H */
