#pragma once

#include "homogeneity_data_source.h"
#include "inference/controllers/categorical_table_cell.h"
#include "inference/controllers/dynamic_cells_data_source.h"
#include "inference/models/homogeneity_test.h"

namespace Inference {

class ResultsHomogeneityController;

using ResultsHomogeneityInnerCellsDataSource = DynamicCellsDataSource<
    InferenceEvenOddBufferCell,
    HomogeneityTableDimensions::k_numberOfInnerReusableCells>;

class ResultsHomogeneityTableCell
    : public CategoricalTableCell,
      public HomogeneityTableDataSource,
      public ResultsHomogeneityInnerCellsDataSource {
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
    return m_inference->numberOfDataRows() + (m_mode == Mode::ExpectedValue);
  }
  int innerNumberOfColumns() const override {
    return m_inference->numberOfDataColumns() + (m_mode == Mode::ExpectedValue);
  }
  Escher::HighlightCell* innerCell(int i) override {
    return DynamicCellsDataSource<
        InferenceEvenOddBufferCell,
        HomogeneityTableDimensions::k_numberOfInnerReusableCells>::cell(i);
  }
  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  // DynamicCellsDataSource
  void destroyCells() override;

  HomogeneityTest* m_inference;
  Mode m_mode;
  ResultsHomogeneityController* m_resultsTableController;
};

}  // namespace Inference
