#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULT_GOODNESS_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULT_GOODNESS_TABLE_CELL_H

#include "inference/models/statistic/goodness_test.h"
#include "inference/statistic/categorical_controller.h"

namespace Inference {

/* This is the controller for the table in the Contributions tab in
 * the GOF Test results. It contains 3 columns: Expected ; Observed ;
 * Contributions. */
class ResultGoodnessContributionsTable
    : public CategoricalTableCell,
      public CategoricalTableViewDataSource,
      public DynamicCellsDataSource<
          InferenceEvenOddBufferCell,
          k_goodnessContributionsTableNumberOfReusableCells>,
      public DynamicCellsDataSourceDelegate<InferenceEvenOddBufferCell> {
 public:
  ResultGoodnessContributionsTable(
      Escher::Responder* parent, CategoricalController* parentController,
      GoodnessTest* statistic, Escher::ScrollViewDelegate* scrollViewDelegate);

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int column) override;
  Escher::HighlightCell* reusableCell(int index, int type) override {
    return cell(index);
  }
  int reusableCellCount(int type) const override {
    return k_goodnessContributionsTableNumberOfReusableCells;
  }

  // DynamicCellsDataSource
  Escher::SelectableTableView* tableView() override {
    return &m_selectableTableView;
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
    return 1 + m_statistic->numberOfValuePairs();
  }
  int numberOfColumns() const override { return 3; }
  int typeAtLocation(int column, int row) const override { return 0; }

  static constexpr int ContributionColumnIndex = 2;

 private:
  static constexpr I18n::Message k_titles[] = {I18n::Message::Observed,
                                               I18n::Message::Expected,
                                               I18n::Message::Contributions};

  static constexpr KDCoordinate k_columnsWidth[] = {95, 95, 100};

  GoodnessTest* m_statistic;
  CategoricalController* m_parentController;
};
}  // namespace Inference

#endif
