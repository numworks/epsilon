#include "results_anova_table_cell.h"

#include <omg/unreachable.h>

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
  }
  CategoricalTableCell::handleResponderChainEvent(event);
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
  assert(row >= 0 && row < k_numberOfInnerRows);
  assert(column >= 0 && column < k_numberOfInnerColumns);

  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);

  const ANOVATest::CalculatedValues& values =
      (column == 0) ? m_inference->results().between
                    : m_inference->results().within;

  switch (row) {
    case 0:
      PrintValueInTextHolder(values.sumOfSquares, myCell);
      break;
    case 1:
      PrintValueInTextHolder(values.degreesOfFreedom, myCell);
      break;
    case 2:
      PrintValueInTextHolder(values.meanSquares, myCell);
      break;
    default:
      OMG::unreachable();
  }

  myCell->setEven(row % 2 == 1);
}

CategoricalController* ResultsANOVATableCell::categoricalController() {
  return m_resultsTableController;
}

}  // namespace Inference
