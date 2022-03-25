#ifndef PROBABILITY_GUI_RESULT_HOMOGENEITY_TABLE_CELL_H
#define PROBABILITY_GUI_RESULT_HOMOGENEITY_TABLE_CELL_H

#include "probability/abstract/homogeneity_data_source.h"
#include "probability/gui/categorical_table_cell.h"
#include "probability/models/statistic/homogeneity_test.h"

namespace Probability {

class ResultHomogeneityTableCell : public CategoricalTableCell, public HomogeneityTableDataSource, public DynamicCellsDataSource<Escher::EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells> {
public:
  ResultHomogeneityTableCell(Escher::Responder * parentResponder, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, HomogeneityTest * test);

  // EditableCategoricalTableCell
  CategoricalTableViewDataSource * tableViewDataSource() override { return this; }

  // DataSource
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView * tableView() override { return &m_selectableTableView; }

private:
  // HomogeneityTableViewDataSource
  int innerNumberOfRows() const override { return m_statistic->numberOfResultRows() + 1; }
  int innerNumberOfColumns() const override { return m_statistic->numberOfResultColumns() + 1; }
  Escher::HighlightCell * innerCell(int i) override { return DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::cell(i); }
  void willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicCellsDataSource
  void createCells() override;
  void destroyCells() override;

  HomogeneityTest * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_INPUT_HOMOGENEITY_TABLE_CELL_H */
