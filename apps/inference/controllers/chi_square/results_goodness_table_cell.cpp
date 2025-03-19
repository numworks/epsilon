#include "results_goodness_table_cell.h"

#include <poincare/print.h>

namespace Inference {

ResultGoodnessContributionsTable::ResultGoodnessContributionsTable(
    Escher::Responder* parent, CategoricalController* parentController,
    GoodnessTest* statistic, Escher::ScrollViewDelegate* scrollViewDelegate)
    : CategoricalTableCell(parent, this, scrollViewDelegate),
      DynamicCellsDataSource<InferenceEvenOddBufferCell,
                             k_goodnessContributionsTableNumberOfReusableCells>(
          this),
      m_statistic(statistic),
      m_parentController(parentController) {
  m_selectableTableView.margins()->setBottom(0);
}

// View
void ResultGoodnessContributionsTable::drawRect(KDContext* ctx,
                                                KDRect rect) const {
  CategoricalTableCell::drawRect(ctx, rect);
  // Draw over the next cell border to hide it
  ctx->fillRect(
      KDRect(0, bounds().height() - Escher::Metric::CellSeparatorThickness,
             bounds().width(), Escher::Metric::CellSeparatorThickness),
      m_selectableTableView.backgroundColor());
}

void ResultGoodnessContributionsTable::fillCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);
  myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);

  if (row == 0) {
    myCell->setText(I18n::translate(k_titles[column]));
    myCell->setEven(true);
  } else {
    fillValueCellForLocation(myCell, myCell, column, row - 1, m_statistic);
  }
}

// TableViewDataSource

KDCoordinate ResultGoodnessContributionsTable::nonMemoizedColumnWidth(
    int column) {
  return k_columnsWidth[column];
}

}  // namespace Inference
