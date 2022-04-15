#ifndef PROBABILITY_GUI_RESULT_HOMOGENEITY_TABLE_CELL_H
#define PROBABILITY_GUI_RESULT_HOMOGENEITY_TABLE_CELL_H

#include "inference/statistic/chi_square_test/categorical_table_cell.h"
#include "inference/statistic/chi_square_test/homogeneity_data_source.h"
#include "inference/models/statistic/homogeneity_test.h"

namespace Inference {

class ResultHomogeneityTableCell : public CategoricalTableCell, public HomogeneityTableDataSource, public DynamicCellsDataSource<Escher::EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells> {
public:
  ResultHomogeneityTableCell(Escher::Responder * parentResponder, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, HomogeneityTest * test);

  // Responder
  void didBecomeFirstResponder() override;

  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;

  // EditableCategoricalTableCell
  CategoricalTableViewDataSource * tableViewDataSource() override { return this; }

  // DataSource
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView * tableView() override { return &m_selectableTableView; }

private:
  constexpr static KDCoordinate k_titleHeight = 24;

  // HomogeneityTableViewDataSource
  int innerNumberOfRows() const override { return m_statistic->numberOfResultRows() + 1; }
  int innerNumberOfColumns() const override { return m_statistic->numberOfResultColumns() + 1; }
  Escher::HighlightCell * innerCell(int i) override { return DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::cell(i); }
  void willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicCellsDataSource
  void createCells() override;
  void destroyCells() override;

  // View
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override;
  void layoutSubviews(bool force = false) override;
  KDSize minimalSizeForOptimalDisplay() const override;

  HomogeneityTest * m_statistic;
  Escher::MessageTextView m_title;
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_INPUT_HOMOGENEITY_TABLE_CELL_H */
