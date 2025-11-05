#pragma once

#include "categorical_table_cell.h"
#include "dynamic_cells_data_source.h"
#include "inference/controllers/tables/anova_table_dimensions.h"
#include "inference/models/anova_test.h"
#include "results_anova_data_source.h"

namespace Inference {

class ResultsANOVAController;

class ResultsANOVATableCell
    : public CategoricalTableCell,
      public ResultsANOVADataSource,
      public DynamicCellsDataSource<Escher::SmallFontEvenOddBufferTextCell> {
 public:
  ResultsANOVATableCell(Escher::Responder* parentResponder, ANOVATest* test,
                        ResultsANOVAController* resultsTableController,
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

  int numberOfDynamicCells() override {
    return ANOVATableDimensions::k_numberOfResultsHeaderCells +
           ANOVATableDimensions::k_numberOfResultsInnerCells;
  }

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  Escher::HighlightCell* reusableHeaderCell(int i) override {
    return dynamicCell(i);
  }
  Escher::HighlightCell* reusableInnerCell(int i) override {
    return dynamicCell(i + ANOVATableDimensions::k_numberOfResultsHeaderCells);
  }

 private:
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row);
  CategoricalController* categoricalController() override;

  ANOVATest* m_inference;
  ResultsANOVAController* m_resultsTableController;
};

}  // namespace Inference
