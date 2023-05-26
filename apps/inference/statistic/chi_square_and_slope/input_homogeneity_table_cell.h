#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_TABLE_CELL_H

#include "inference/models/statistic/homogeneity_test.h"
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "inference/statistic/chi_square_and_slope/homogeneity_data_source.h"

namespace Inference {

class InputHomogeneityController;

class InputHomogeneityTableCell
    : public InputCategoricalTableCell,
      public HomogeneityTableDataSource,
      public DynamicCellsDataSource<
          InferenceEvenOddEditableCell,
          k_homogeneityTableNumberOfReusableInnerCells> {
 public:
  InputHomogeneityTableCell(
      Escher::Responder* parentResponder, HomogeneityTest* test,
      InputHomogeneityController* inputHomogeneityController);

  // Responder
  void didBecomeFirstResponder() override;

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // DataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  // SelectableTableViewDelegate
  bool canStoreContentOfCellAtLocation(Escher::SelectableTableView* t, int col,
                                       int row) const override {
    return col > 0 && row > 0;
  }

 private:
  // ClearColumnHelper
  int fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override { return column - 1; }

  // HomogeneityTableViewDataSource
  Escher::HighlightCell* innerCell(int i) override {
    return DynamicCellsDataSource<
        InferenceEvenOddEditableCell,
        k_homogeneityTableNumberOfReusableInnerCells>::cell(i);
  }
  void willDisplayInnerCellAtLocation(Escher::HighlightCell* cell, int column,
                                      int row) override;
  CategoricalController* categoricalController() override;

  // DynamicCellsDataSource
  void createCells() override;
  void destroyCells() override;
  InputHomogeneityController* m_inputHomogeneityController;
};

}  // namespace Inference

#endif
