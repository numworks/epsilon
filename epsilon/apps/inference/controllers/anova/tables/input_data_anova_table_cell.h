#pragma once

#include "anova_table_dimensions.h"
#include "inference/controllers/tables/categorical_table_cell.h"
#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/controllers/tables/one_header_row_table_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

class InputDataANOVAController;

class InputDataANOVATableCell
    : public InputCategoricalTableCell,
      public OneHeaderRowTableDataSource,
      public DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                          InferenceEvenOddEditableCell> {
 public:
  constexpr static int k_maxNumberOfColumns =
      ANOVATableDimensions::k_inputShape.inner.columns;
  constexpr static int k_maxNumberOfInnerRows =
      ANOVATableDimensions::k_inputShape.inner.rows;
  constexpr static int k_maxNumberOfRows = k_maxNumberOfInnerRows + 1;

  constexpr static int k_columnWidth = ANOVATableDimensions::k_columnWidth;

  InputDataANOVATableCell(Escher::Responder* parentResponder, ANOVATest* test,
                          InputDataANOVAController* InputDataANOVAController,
                          Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // CategoricalTableViewDataSource

  int maxNumberOfRows() const override { return k_maxNumberOfRows; }
  int maxNumberOfColumn() const override { return k_maxNumberOfColumns; }

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    assert(column >= 0 && column < k_maxNumberOfColumns);
    return k_columnWidth;
  }

  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  bool canStoreCellAtLocation(int column, int row) override {
    assert(column >= 0 && row >= 0);
    assert(column < k_maxNumberOfColumns && row < k_maxNumberOfRows);
    return column >= 0 && row > 0;
  }

  // DoubleDynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  void initInnerCell(InferenceEvenOddEditableCell* cell) override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  // DataSource

  ReusableCellCounts reusableCellCounts() const override {
    return ANOVATableDimensions::k_inputShape.reusable;
  }

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return cellType1(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return cellType2(i);
  }

  // CategoricalTableViewDataSource

  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }

  // DoubleDynamicCellsDataSource

  int numberOfDynamicHeaderCells() const override {
    return reusableCellCounts().categories.header;
  }
  int numberOfDynamicInnerCells() const override {
    return reusableCellCounts().categories.inner;
  }

 private:
  constexpr static int k_headerTranslationBufferSize = 20;

  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override { return column; }

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row);
  CategoricalController* categoricalController() override;

  InputDataANOVAController* m_InputDataANOVAController;
};

}  // namespace Inference
