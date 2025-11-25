#pragma once

#include "inference/controllers/tables/categorical_table_cell.h"
#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/controllers/tables/two_header_columns_one_header_row_table_data_source.h"
#include "inference/models/anova_test.h"
#include "table_dimensions.h"

namespace Inference::ANOVA {

class BetweenWithinController;

class ResultsBetweenWithinTableCell
    : public CategoricalTableCell,
      public TwoHeaderColumnsOneHeaderRowTableDataSource,
      public DynamicCellsDataSource<Escher::SmallFontEvenOddBufferTextCell> {
 public:
  constexpr static int k_numberOfInnerColumns =
      TableDimensions::k_resultBetweenWithinShape.inner.columns;
  constexpr static int k_numberOfInnerRows =
      TableDimensions::k_resultBetweenWithinShape.inner.rows;

  ResultsBetweenWithinTableCell(
      Escher::Responder* parentResponder, ANOVATest* test,
      BetweenWithinController* betweenWithinTableController,
      Escher::ScrollViewDelegate* scrollViewDelegate);

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  int numberOfDynamicCells() const override {
    return reusableCellCounts().categories.header +
           reusableCellCounts().categories.inner;
  }

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  // DataSource

  ReusableCellCounts reusableCellCounts() const override {
    return TableDimensions::k_resultBetweenWithinShape.reusable;
  }

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return dynamicCell(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return dynamicCell(i + reusableCellCounts().categories.header);
  }

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    assert(column >= 0 && column < numberOfColumns());
    return column < numberOfHeaderColumns()
               ? column == 0 ? TableDimensions::k_resultTitleColumnWidth
                             : TableDimensions::k_symbolColumnWidth
               : TableDimensions::k_columnWidth;
  }

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return k_numberOfInnerRows; }
  int innerNumberOfColumns() const override { return k_numberOfInnerColumns; }

  void fillColumnTitleForLocation(Escher::HighlightCell* cell,
                                  int innerColumn) override;
  void fillRowTitleForLocation(Escher::HighlightCell* cell,
                               int innerRow) override;
  void fillRowSymbolForLocation(Escher::HighlightCell* cell,
                                int innerRow) override;

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int innerColumn,
                                int innerRow) override;

 private:
  CategoricalController* categoricalController() override;

  ANOVATest* m_inference;
  BetweenWithinController* m_betweenWithinTableController;
};

}  // namespace Inference::ANOVA
