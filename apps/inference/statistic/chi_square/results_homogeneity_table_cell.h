#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_HOMOGENEITY_TABLE_CELL_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_RESULTS_HOMOGENEITY_TABLE_CELL_H

#include "homogeneity_data_source.h"
#include "inference/models/statistic/homogeneity_test.h"
#include "inference/statistic/categorical_table_cell.h"

namespace Inference {

class ResultsHomogeneityController;

class ResultsHomogeneityTableCell
    : public CategoricalTableCell,
      public HomogeneityTableDataSource,
      public DynamicCellsDataSource<
          InferenceEvenOddBufferCell,
          k_homogeneityTableNumberOfReusableInnerCells> {
 public:
  ResultsHomogeneityTableCell(
      Escher::Responder* parentResponder, HomogeneityTest* test,
      ResultsHomogeneityController* resultsTableController,
      Escher::ScrollViewDelegate* scrollViewDelegate);

  enum class Mode : bool { ExpectedValue, Contribution };
  void setMode(Mode mode) { m_mode = mode; }

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
  void createCells() override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  // HomogeneityTableViewDataSource
  /* The totals are not displayed when in Contribution mode. */
  int innerNumberOfRows() const override {
    return m_statistic->numberOfResultRows() + (m_mode == Mode::ExpectedValue);
  }
  int innerNumberOfColumns() const override {
    return m_statistic->numberOfResultColumns() +
           (m_mode == Mode::ExpectedValue);
  }
  Escher::HighlightCell* innerCell(int i) override {
    return DynamicCellsDataSource<
        InferenceEvenOddBufferCell,
        k_homogeneityTableNumberOfReusableInnerCells>::cell(i);
  }
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  // DynamicCellsDataSource
  void destroyCells() override;

  HomogeneityTest* m_statistic;
  Mode m_mode;
  ResultsHomogeneityController* m_resultsTableController;
};

}  // namespace Inference

#endif
