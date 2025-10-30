#pragma once

#include "anova_data_source.h"
#include "categorical_table_cell.h"
#include "inference/models/anova_test.h"

namespace Inference {

class InputANOVAController;

class InputANOVATableCell
    : public InputCategoricalTableCell,
      public ANOVATableDataSource,
      public DynamicCellsDataSource<InferenceEvenOddEditableCell> {
 public:
  InputANOVATableCell(Escher::Responder* parentResponder, ANOVATest* test,
                      InputANOVAController* inputANOVAController,
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
    assert(column >= 0 && row >= 0);
    assert(column < k_maxNumberOfColumns && row < k_maxNumberOfRows);
    return column >= 0 && row > 0;
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
  int relativeColumn(int column) const override { return column; }

  // ANOVATableViewDataSource
  Escher::HighlightCell* innerCell(int i) override {
    return DynamicCellsDataSource<InferenceEvenOddEditableCell>::cell(i);
  }
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  // DynamicCellsDataSource
  void destroyCells() override;
  InputANOVAController* m_inputANOVAController;
};

}  // namespace Inference
