#pragma once

#include "anova_table_dimensions.h"
#include "categorical_table_cell.h"
#include "dynamic_cells_data_source.h"
#include "inference/models/anova_test.h"
#include "one_column_header_table_data_source.h"

namespace Inference {

class InputANOVAController;

class InputANOVATableCell
    : public InputCategoricalTableCell,
      public OneColumnHeaderTableDataSource,
      public DoubleDynamicCellsDataSource<InferenceEvenOddBufferCell,
                                          InferenceEvenOddEditableCell> {
 public:
  constexpr static int k_maxNumberOfColumns =
      ANOVATableDimensions::k_inputShape.inner.columns;
  constexpr static int k_maxNumberOfInnerRows =
      ANOVATableDimensions::k_inputShape.inner.rows;
  constexpr static int k_maxNumberOfRows = k_maxNumberOfInnerRows + 1;

  constexpr static int k_columnWidth = ANOVATableDimensions::k_columnWidth;

  InputANOVATableCell(Escher::Responder* parentResponder, ANOVATest* test,
                      InputANOVAController* inputANOVAController,
                      Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return m_numberOfRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }

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

  void initCellType2(InferenceEvenOddEditableCell* cell) override;

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

  // DoubleDynamicCellsDataSource

  int numberOfDynamicCellsType1() const override {
    return reusableCellCounts().categories.header;
  }
  int numberOfDynamicCellsType2() const override {
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

  InputANOVAController* m_inputANOVAController;
  I18n::Message m_headerPrefix;
};

}  // namespace Inference
