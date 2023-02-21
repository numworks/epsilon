#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_TABLE_CELL_H

#include "inference/models/statistic/homogeneity_test.h"
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "inference/statistic/chi_square_and_slope/homogeneity_data_source.h"

namespace Inference {

class InputHomogeneityTableCell
    : public EditableCategoricalTableCell,
      public HomogeneityTableDataSource,
      public DynamicCellsDataSource<
          Escher::EvenOddEditableTextCell,
          k_homogeneityTableNumberOfReusableInnerCells> {
 public:
  InputHomogeneityTableCell(
      Escher::Responder* parentResponder,
      DynamicSizeTableViewDataSourceDelegate*
          dynamicSizeTableViewDataSourceDelegate,
      Escher::SelectableTableViewDelegate* selectableTableViewDelegate,
      HomogeneityTest* test);

  // Responder
  void didBecomeFirstResponder() override;

  // EditableCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // DataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }
  void willDisplayCellAtLocation(Escher::HighlightCell* cell, int column,
                                 int row) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

 private:
  // ClearColumnHelper
  int fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumnIndex(int columnIndex) const override {
    return columnIndex - 1;
  }

  // HomogeneityTableViewDataSource
  Escher::HighlightCell* innerCell(int i) override {
    return DynamicCellsDataSource<
        Escher::EvenOddEditableTextCell,
        k_homogeneityTableNumberOfReusableInnerCells>::cell(i);
  }
  void willDisplayInnerCellAtLocation(Escher::HighlightCell* cell, int column,
                                      int row) override;

  // DynamicCellsDataSource
  void createCells() override;
  void destroyCells() override;
};

}  // namespace Inference

#endif
