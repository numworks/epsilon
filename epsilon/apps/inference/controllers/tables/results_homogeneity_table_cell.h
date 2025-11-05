#pragma once

#include "categorical_table_cell.h"
#include "dynamic_cells_data_source.h"
#include "homogeneity_data_source.h"
#include "inference/models/homogeneity_test.h"

namespace Inference {

class ResultsHomogeneityController;

class ResultsHomogeneityTableCell
    : public CategoricalTableCell,
      public HomogeneityTableDataSource,
      public DynamicCellsDataSource<InferenceEvenOddBufferCell> {
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

  int numberOfCells() override {
    return HomogeneityTableDimensions::k_numberOfHeaderReusableCells +
           HomogeneityTableDimensions::k_numberOfInnerReusableCells;
  }

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

  Escher::HighlightCell* headerCell(int i) override { return cell(i); }
  Escher::HighlightCell* innerCell(int i) override {
    return cell(i + HomogeneityTableDimensions::k_numberOfHeaderReusableCells);
  }

  void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                int row) override;
  CategoricalController* categoricalController() override;

  HomogeneityTest* m_inference;
  Mode m_mode;
  ResultsHomogeneityController* m_resultsTableController;
};

}  // namespace Inference
