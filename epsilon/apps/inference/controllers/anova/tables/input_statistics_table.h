#pragma once

#include "inference/controllers/tables/categorical_table_cell.h"
#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/controllers/tables/two_header_columns_one_header_row_table_data_source.h"
#include "inference/models/anova_test.h"
#include "table_dimensions.h"

namespace Inference::ANOVA {

class InputStatisticsController;

class InputStatisticsTable : public InputCategoricalTableCell,
                             public TwoHeaderColumnsOneHeaderRowTableDataSource,
                             public DoubleDynamicCellsDataSource<
                                 Escher::SmallFontEvenOddBufferTextCell,
                                 InferenceEvenOddEditableCell> {
 public:
  constexpr static int k_numberOfInnerColumns =
      TableDimensions::k_inputStatisticsShape.inner.columns;
  constexpr static int k_numberOfInnerRows =
      TableDimensions::k_inputStatisticsShape.inner.rows;

  constexpr static int k_columnWidth = TableDimensions::k_columnWidth;

  InputStatisticsTable(Escher::Responder* parentResponder, ANOVATest* test,
                       InputStatisticsController* InputDataController,
                       Escher::ScrollViewDelegate* scrollViewDelegate);

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    assert(column >= 0 && column < numberOfColumns());
    return column < numberOfHeaderColumns()
               ? column == 0 ? TableDimensions::k_resultTitleColumnWidth
                             : TableDimensions::k_symbolColumnWidth
               : TableDimensions::k_columnWidth;
  }

  // DoubleDynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  void initInnerCell(innerCellType* cell) override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  // DataSource

  ReusableCellCounts reusableCellCounts() const override {
    return TableDimensions::k_inputStatisticsShape.reusable;
  }

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return dynamicHeaderCell(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return dynamicInnerCell(i);
  }

  // DoubleDynamicCellsDataSource

  int numberOfDynamicHeaderCells() const override {
    return reusableCellCounts().categories.header;
  }
  int numberOfDynamicInnerCells() const override {
    return reusableCellCounts().categories.inner;
  }

  void fillColumnTitleForLocation(Escher::HighlightCell* cell,
                                  int innerColumn) override;
  void fillRowTitleForLocation(Escher::HighlightCell* cell,
                               int innerRow) override;
  void fillRowSymbolForLocation(Escher::HighlightCell* cell,
                                int innerRow) override;

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int innerColumn,
                                int innerRow) override;

 private:
  constexpr static int k_headerTranslationBufferSize = 20;

  // ClearColumnHelper
  size_t fillColumnName(int column, char* buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumn(int column) const override {
    return column - numberOfHeaderColumns();
  }
  int relativeRow(int row) const override { return row - numberOfHeaderRows(); }

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return k_numberOfInnerRows; }
  int innerNumberOfColumns() const override { return m_numberOfColumns; }

  CategoricalController* categoricalController() override;

  InputStatisticsController* m_inputStatisticsController;
};

}  // namespace Inference::ANOVA
