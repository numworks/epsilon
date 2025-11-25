#pragma once

#include "homogeneity_data_source.h"
#include "inference/controllers/tables/categorical_table_cell.h"
#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/models/homogeneity_test.h"

namespace Inference {

class InputHomogeneityController;

class InputHomogeneityTable
    : public InputCategoricalTableCell,
      public HomogeneityTableDataSource,
      public DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                          InferenceEvenOddEditableCell> {
 public:
  InputHomogeneityTable(Escher::Responder* parentResponder,
                        HomogeneityTest* test,
                        InputHomogeneityController* inputHomogeneityController,
                        Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // CategoricalTableViewDataSource
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  bool canStoreCellAtLocation(int column, int row) override {
    return column > 0 && row > 0;
  }

  // DoubleDynamicCellsDataSource

  int numberOfDynamicHeaderCells() const override {
    return HomogeneityTableDimensions::k_numberOfHeaderReusableCells;
  }
  int numberOfDynamicInnerCells() const override {
    return HomogeneityTableDimensions::k_numberOfInnerReusableCells;
  }

  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  void initInnerCell(InferenceEvenOddEditableCell* cell) override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }

 private:
  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override {
    return column - numberOfHeaderColumns();
  }

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return dynamicHeaderCell(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return dynamicInnerCell(i);
  }

  InputHomogeneityController* m_inputHomogeneityController;
};

}  // namespace Inference
