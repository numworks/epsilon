#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_INPUT_HOMOGENEITY_TABLE_CELL_H

#include "homogeneity_data_source.h"
#include "inference/models/statistic/homogeneity_test.h"
#include "inference/statistic/categorical_table_cell.h"

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
      InputHomogeneityController* inputHomogeneityController,
      Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // DataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  bool canStoreCellAtLocation(int column, int row) override {
    return column > 0 && row > 0;
  }

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }
  void createCells() override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override { return column - 1; }

  // HomogeneityTableViewDataSource
  Escher::HighlightCell* innerCell(int i) override {
    return DynamicCellsDataSource<
        InferenceEvenOddEditableCell,
        k_homogeneityTableNumberOfReusableInnerCells>::cell(i);
  }
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  // DynamicCellsDataSource
  void destroyCells() override;
  InputHomogeneityController* m_inputHomogeneityController;
};

}  // namespace Inference

#endif
