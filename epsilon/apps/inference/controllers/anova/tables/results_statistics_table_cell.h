#pragma once

#include "anova_table_dimensions.h"
#include "inference/controllers/tables/categorical_table_cell.h"
#include "inference/controllers/tables/dynamic_cells_data_source.h"
#include "inference/controllers/tables/one_header_column_one_header_row_table_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference::ANOVA {

class StatisticsController;

class ResultsStatisticsTableCell
    : public CategoricalTableCell,
      public OneHeaderColumnOneHeaderRowTableDataSource,
      public DynamicCellsDataSource<Escher::SmallFontEvenOddBufferTextCell> {
 public:
  constexpr static int k_numberOfInnerColumns =
      ANOVATableDimensions::k_resultGroupStatisticsShape.inner.columns;
  constexpr static int k_numberOfInnerRows =
      ANOVATableDimensions::k_resultGroupStatisticsShape.inner.rows;

  ResultsStatisticsTableCell(Escher::Responder* parentResponder,
                             ANOVATest* test,
                             StatisticsController* statisticsTableController,
                             Escher::ScrollViewDelegate* scrollViewDelegate);

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // InputCategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }

  // DataSource
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  bool canStoreCellAtLocation(int column, int row) override {
    return column > 0 && row > 0;
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
    return ANOVATableDimensions::k_resultGroupStatisticsShape.reusable;
  }

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return dynamicCell(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return dynamicCell(i + reusableCellCounts().categories.header);
  }

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return column == 0 ? ANOVATableDimensions::k_resultTitleColumnWidth
                       : ANOVATableDimensions::k_columnWidth;
  }

 private:
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row);
  CategoricalController* categoricalController() override;

  // CategoricalTableViewDataSource
  int innerNumberOfRows() const override { return k_numberOfInnerRows; }
  int innerNumberOfColumns() const override {
    return m_inference->numberOfGroups();
  }

  ANOVATest* m_inference;
  StatisticsController* m_statisticsResultsController;
};

}  // namespace Inference::ANOVA
