#include "result_homogeneity_table_cell.h"

using namespace Escher;

namespace Inference {

ResultHomogeneityTableCell::ResultHomogeneityTableCell(Escher::Responder * parentResponder, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, HomogeneityTest * test) :
  CategoricalTableCell(parentResponder, this, selectableTableViewDelegate),
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>(this),
  m_statistic(test),
  m_title(KDFont::SmallFont,
      I18n::Message::HomogeneityResultsTitle,
      KDContext::k_alignCenter,
      KDContext::k_alignCenter,
      Escher::Palette::GrayVeryDark,
      Escher::Palette::WallScreenDark),
  m_mode(Mode::Contribution) // FIXME Turn to ExpectedValue once tabs are implemented
{
  m_selectableTableView.setTopMargin(0);
  m_selectableTableView.setBottomMargin(Metric::CellSeparatorThickness);
}

void ResultHomogeneityTableCell::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectColumn(1);
  }
  // The number of data might have changed
  if (selectedRow() >= numberOfRows() || selectedColumn() >= numberOfColumns()) {
    selectRow(1);
    selectColumn(1);
  }
  CategoricalTableCell::didBecomeFirstResponder();
}

void ResultHomogeneityTableCell::drawRect(KDContext * ctx, KDRect rect) const {
  CategoricalTableCell::drawRect(ctx, rect);
  // Draw over the next cell border to hide it
  ctx->fillRect(KDRect(0, bounds().height() - Metric::CellSeparatorThickness, bounds().width(), Metric::CellSeparatorThickness), m_selectableTableView.backgroundColor());
}

void ResultHomogeneityTableCell::willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) {
  if (m_mode == Mode::ExpectedValue && ((column == 0 && row == innerNumberOfRows()) || (row == 0 && column == innerNumberOfColumns()))) {
    // Override to display "Total" instead
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    myCell->setText(I18n::translate(I18n::Message::Total));
    myCell->setEven(row % 2 == 0);
  } else {
    HomogeneityTableDataSource::willDisplayCellAtLocation(cell, column, row);
  }
}

void ResultHomogeneityTableCell::willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) {
  EvenOddBufferTextCell * myCell = static_cast<EvenOddBufferTextCell *>(cell);

  double value;
  if (m_mode == Mode::ExpectedValue) {
    if (column == m_statistic->numberOfResultColumns() && row == m_statistic->numberOfResultRows()) {
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

void ResultHomogeneityTableCell::createCells() {
  if (DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::createCellsWithOffset(0);
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells * sizeof(EvenOddBufferTextCell));
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_delegate->tableView()->reloadData(false, false);
  }
}

void ResultHomogeneityTableCell::destroyCells() {
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

Escher::View * ResultHomogeneityTableCell::subviewAtIndex(int i) {
  Escher::View * views[] = {&m_selectableTableView, &m_title};
  return views[i];
}

void ResultHomogeneityTableCell::layoutSubviews(bool force) {
  m_title.setFrame(KDRect(0, 0, bounds().width(), k_titleHeight), force);
  m_selectableTableView.setFrame(KDRect(0, k_titleHeight, bounds().width(), bounds().height() - k_titleHeight - Metric::CellSeparatorThickness), force);
}

KDSize ResultHomogeneityTableCell::minimalSizeForOptimalDisplay() const {
  return m_selectableTableView.minimalSizeForOptimalDisplay() + KDSize(0, k_titleHeight);
}

}  // namespace Inference
