#pragma once

#include "categorical_table_cell.h"
#include "dynamic_cells_data_source.h"
#include "homogeneity_data_source.h"
#include "inference/models/homogeneity_test.h"

namespace Inference {

class InputHomogeneityController;

class InputHomogeneityTableCell
    : public InputCategoricalTableCell,
      public HomogeneityTableDataSource,
      public DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                          InferenceEvenOddEditableCell> {
 public:
  InputHomogeneityTableCell(
      Escher::Responder* parentResponder, HomogeneityTest* test,
      InputHomogeneityController* inputHomogeneityController,
      Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  bool canStoreCellAtLocation(int column, int row) override {
    return column > 0 && row > 0;
  }

  // DoubleDynamicCellsDataSource

  int numberOfDynamicCellsType1() const override {
    return HomogeneityTableDimensions::k_numberOfHeaderReusableCells;
  }
  int numberOfDynamicCellsType2() const override {
    return HomogeneityTableDimensions::k_numberOfInnerReusableCells;
  }

  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  void initCellType2(InferenceEvenOddEditableCell* cell) override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override { return column - 1; }

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return cellType1(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return cellType2(i);
  }

  InputHomogeneityController* m_inputHomogeneityController;
};

}  // namespace Inference
