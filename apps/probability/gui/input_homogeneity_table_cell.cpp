#include "input_homogeneity_table_cell.h"

using namespace Escher;

namespace Probability {

InputHomogeneityTableCell::InputHomogeneityTableCell(Escher::Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, HomogeneityTest * test) :
  EditableCategoricalTableCell(parentResponder, this, selectableTableViewDelegate, dynamicSizeTableViewDataSourceDelegate, test),
  DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>(this)
{
}

void InputHomogeneityTableCell::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectColumn(1);
  }
  EditableCategoricalTableCell::didBecomeFirstResponder();
}

void InputHomogeneityTableCell::willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) {
  Escher::EvenOddEditableTextCell * myCell = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(), myCell, column, row, m_statistic);
}

void InputHomogeneityTableCell::createCells() {
  // We could equivalently use DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::m_cells
  if (DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::createCellsWithOffset(0);
    DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells * sizeof(EvenOddBufferTextCell));
    // We could equivalently use DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::m_delegate
    DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::m_delegate->tableView()->reloadData(false, false);
  }
}

void InputHomogeneityTableCell::destroyCells() {
  DynamicCellsDataSource<EvenOddEditableTextCell, k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

}  // namespace Probability
