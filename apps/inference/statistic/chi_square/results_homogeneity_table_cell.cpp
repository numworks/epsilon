#include "results_homogeneity_table_cell.h"

#include "results_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

ResultsHomogeneityTableCell::ResultsHomogeneityTableCell(
    Escher::Responder* parentResponder, HomogeneityTest* test,
    ResultsHomogeneityController* resultsTableController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : CategoricalTableCell(parentResponder, this, scrollViewDelegate),
      DynamicCellsDataSource<InferenceEvenOddBufferCell,
                             k_homogeneityTableNumberOfReusableInnerCells>(
          this),
      m_statistic(test),
      m_mode(Mode::ExpectedValue),
      m_resultsTableController(resultsTableController) {
  m_selectableTableView.margins()->setBottom(Metric::CellSeparatorThickness);
}

void ResultsHomogeneityTableCell::handleResponderChainEvent(
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

void ResultsHomogeneityTableCell::drawRect(KDContext* ctx, KDRect rect) const {
  CategoricalTableCell::drawRect(ctx, rect);
  // Draw over the next cell border to hide it
  ctx->fillRect(KDRect(0, bounds().height() - Metric::CellSeparatorThickness,
                       bounds().width(), Metric::CellSeparatorThickness),
                m_selectableTableView.backgroundColor());
}

void ResultsHomogeneityTableCell::fillCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  if (m_mode == Mode::ExpectedValue &&
      ((column == 0 && row == innerNumberOfRows()) ||
       (row == 0 && column == innerNumberOfColumns()))) {
    // Override to display "Total" instead
    InferenceEvenOddBufferCell* myCell =
        static_cast<InferenceEvenOddBufferCell*>(cell);
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setText(I18n::translate(I18n::Message::Total));
    myCell->setEven(row % 2 == 0);
  } else {
    HomogeneityTableDataSource::fillCellForLocation(cell, column, row);
  }
}

void ResultsHomogeneityTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);

  double value;
  if (m_mode == Mode::ExpectedValue) {
    if (column == m_statistic->numberOfResultColumns() &&
        row == m_statistic->numberOfResultRows()) {
      value = m_statistic->total();
    } else if (column == m_statistic->numberOfResultColumns()) {
      value = m_statistic->rowTotal(row);
    } else if (row == m_statistic->numberOfResultRows()) {
      value = m_statistic->columnTotal(column);
    } else {
      value = m_statistic->expectedValueAtLocation(row, column);
    }
  } else {
    assert(m_mode == Mode::Contribution);
    value = m_statistic->contributionAtLocation(row, column);
  }
  PrintValueInTextHolder(value, myCell);
  myCell->setEven(row % 2 == 1);
}

void ResultsHomogeneityTableCell::createCells() {
  if (DynamicCellsDataSource<
          InferenceEvenOddBufferCell,
          k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<InferenceEvenOddBufferCell,
                           k_homogeneityTableNumberOfReusableHeaderCells>::
        createCellsWithOffset(0);
    DynamicCellsDataSource<InferenceEvenOddBufferCell,
                           k_homogeneityTableNumberOfReusableInnerCells>::
        createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells *
                              sizeof(InferenceEvenOddBufferCell));
  }
}

void ResultsHomogeneityTableCell::destroyCells() {
  DynamicCellsDataSource<
      InferenceEvenOddBufferCell,
      k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<
      InferenceEvenOddBufferCell,
      k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

CategoricalController* ResultsHomogeneityTableCell::categoricalController() {
  return m_resultsTableController;
}

}  // namespace Inference
