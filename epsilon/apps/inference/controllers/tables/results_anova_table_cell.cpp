#include "results_anova_table_cell.h"

#include "inference/controllers/results_anova_controller.h"
#include "inference/models/anova_test.h"
#include "results_anova_data_source.h"

using namespace Escher;

namespace Inference {

ResultsANOVATableCell::ResultsANOVATableCell(
    Escher::Responder* parentResponder, ANOVATest* test,
    ResultsANOVAController* resultsTableController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : CategoricalTableCell(parentResponder, this, scrollViewDelegate),
      m_inference(test),
      m_resultsTableController(resultsTableController) {
  m_selectableTableView.margins()->setBottom(Metric::CellSeparatorThickness);
}

void ResultsANOVATableCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0) {
      selectColumn(1);
    }
    // The number of data might have changed
    if (selectedRow() >= numberOfRows() ||
        selectedColumn() >= numberOfColumns()) {
      selectRow(1);
      selectColumn(1);
    }
    CategoricalTableCell::handleResponderChainEvent(event);
  } else {
    CategoricalTableCell::handleResponderChainEvent(event);
  }
}

void ResultsANOVATableCell::drawRect(KDContext* ctx, KDRect rect) const {
  CategoricalTableCell::drawRect(ctx, rect);
  // Draw over the next cell border to hide it
  ctx->fillRect(KDRect(0, bounds().height() - Metric::CellSeparatorThickness,
                       bounds().width(), Metric::CellSeparatorThickness),
                m_selectableTableView.backgroundColor());
}

void ResultsANOVATableCell::fillCellForLocation(Escher::HighlightCell* cell,
                                                int column, int row) {
  ResultsANOVADataSource::fillCellForLocation(cell, column, row);
}

void ResultsANOVATableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);

  // TODO

  double value = m_inference->pValue();
  PrintValueInTextHolder(value, myCell);
  myCell->setEven(row % 2 == 1);
}

CategoricalController* ResultsANOVATableCell::categoricalController() {
  return m_resultsTableController;
}

}  // namespace Inference
