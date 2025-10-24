#pragma once

#include "categorical_table_cell.h"
#include "dynamic_cells_data_source.h"
#include "inference/controllers/tables/anova_table_dimensions.h"
#include "inference/models/anova_test.h"
#include "results_anova_data_source.h"

namespace Inference {

class ResultsANOVAController;

class ResultsANOVATableCell : public CategoricalTableCell,
                              public ResultsANOVADataSource {
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

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  // ANOVATableViewDataSource
  /* The totals are not displayed when in Contribution mode. */
  int innerNumberOfRows() const override {
    // TODO: is this true?
    return k_maxNumberOfRows;
  }
  int innerNumberOfColumns() const override {
    // TODO: is this true?
    return k_maxNumberOfColumns;
  }
  Escher::HighlightCell* innerCell(int i) override {
    return cell(i + ANOVATableDimensions::k_numberOfResultsHeaderReusableCells);
  }
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  ANOVATest* m_inference;
  ResultsANOVAController* m_resultsTableController;
};

}  // namespace Inference
