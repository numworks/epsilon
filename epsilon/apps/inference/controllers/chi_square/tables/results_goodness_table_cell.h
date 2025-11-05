#pragma once

#include "inference/controllers/tables/categorical_controller.h"
#include "inference/models/goodness_test.h"

namespace Inference {

namespace ResultsGoodnessContributionsDimensions {
static constexpr int k_numberOfReusableCells =
    Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
        Escher::Metric::SmallEditableCellHeight, Escher::Metric::TabHeight) *
    3;
}

/* This is the controller for the table in the Contributions tab in
 * the GOF Test results. It contains 3 columns: Expected ; Observed ;
 * Contributions. */
class ResultGoodnessContributionsTable
    : public CategoricalTableCell,
      public CategoricalTableViewDataSource,
      public DynamicCellsDataSource<InferenceEvenOddBufferCell> {
 public:
  ResultGoodnessContributionsTable(
      Escher::Responder* parent, CategoricalController* parentController,
      GoodnessTest* inference, Escher::ScrollViewDelegate* scrollViewDelegate);

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int column) override;
  Escher::HighlightCell* reusableCell(int index, int type) override {
    return dynamicCell(index);
  }
  int reusableCellCount(int type) const override {
    return ResultsGoodnessContributionsDimensions::k_numberOfReusableCells;
  }

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
  }

  int numberOfDynamicCells() const override {
    return ResultsGoodnessContributionsDimensions::k_numberOfReusableCells;
  }

  // CategoricalTableCell
  CategoricalTableViewDataSource* tableViewDataSource() override {
    return this;
  }
  CategoricalController* categoricalController() override {
    return m_parentController;
  }

  // TableViewDataSource
  int numberOfRows() const override {
    return 1 + m_inference->numberOfDataRows();
  }
  int numberOfColumns() const override { return 3; }
  int typeAtLocation(int column, int row) const override { return 0; }

 private:
  constexpr static I18n::Message k_titles[] = {I18n::Message::Observed,
                                               I18n::Message::Expected,
                                               I18n::Message::Contributions};

  constexpr static KDCoordinate k_columnsWidth[] = {95, 95, 100};

  GoodnessTest* m_inference;
  CategoricalController* m_parentController;
};
}  // namespace Inference
