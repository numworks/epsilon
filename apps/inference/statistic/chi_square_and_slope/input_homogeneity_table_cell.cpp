#include "input_homogeneity_table_cell.h"

#include <shared/column_parameter_controller.h>

using namespace Escher;

namespace Inference {

InputHomogeneityTableCell::InputHomogeneityTableCell(
    Escher::Responder *parentResponder,
    DynamicSizeTableViewDataSourceDelegate
        *dynamicSizeTableViewDataSourceDelegate,
    Escher::SelectableTableViewDelegate *selectableTableViewDelegate,
    HomogeneityTest *test)
    : EditableCategoricalTableCell(
          parentResponder, this, selectableTableViewDelegate,
          dynamicSizeTableViewDataSourceDelegate, test),
      DynamicCellsDataSource<EvenOddEditableTextCell,
                             k_homogeneityTableNumberOfReusableInnerCells>(
          this) {}

void InputHomogeneityTableCell::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectColumn(1);
  }
  EditableCategoricalTableCell::didBecomeFirstResponder();
}

void InputHomogeneityTableCell::willDisplayCellAtLocation(
    Escher::HighlightCell *cell, int column, int row) {
  HomogeneityTableDataSource::willDisplayCellAtLocation(cell, column, row);
  if ((column == 0 && row != 0 && row == numberOfRows() - 1 &&
       row < k_maxNumberOfRows) ||
      (row == 0 && column != 0 && column == numberOfColumns() - 1 &&
       column < k_maxNumberOfColumns)) {
    /* The last column/row is empty and has its title grayed out. */
    Escher::EvenOddBufferTextCell *typedCell =
        static_cast<Escher::EvenOddBufferTextCell *>(cell);
    typedCell->setTextColor(Palette::GrayDark);
  }
}

int InputHomogeneityTableCell::fillColumnName(int column, char *buffer) {
  int length = strlcpy(buffer, I18n::translate(I18n::Message::Group),
                       Shared::ColumnParameterController::k_titleBufferSize);
  buffer[length++] = '0' + column;
  buffer[length] = '\0';
  assert(length < Shared::ColumnParameterController::k_titleBufferSize);
  return length;
}

void InputHomogeneityTableCell::willDisplayInnerCellAtLocation(
    Escher::HighlightCell *cell, int column, int row) {
  Escher::EvenOddEditableTextCell *myCell =
      static_cast<Escher::EvenOddEditableTextCell *>(cell);
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(),
                                 myCell, column, row, tableModel());
}

void InputHomogeneityTableCell::createCells() {
  // We could equivalently use DynamicCellsDataSource<EvenOddEditableTextCell,
  // k_homogeneityTableNumberOfReusableInnerCells>::m_cells
  if (DynamicCellsDataSource<
          EvenOddBufferTextCell,
          k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<EvenOddBufferTextCell,
                           k_homogeneityTableNumberOfReusableHeaderCells>::
        createCellsWithOffset(0);
    DynamicCellsDataSource<EvenOddEditableTextCell,
                           k_homogeneityTableNumberOfReusableInnerCells>::
        createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells *
                              sizeof(EvenOddBufferTextCell));
    // We could equivalently use DynamicCellsDataSource<EvenOddEditableTextCell,
    // k_homogeneityTableNumberOfReusableInnerCells>::m_delegate
    DynamicCellsDataSource<
        EvenOddBufferTextCell,
        k_homogeneityTableNumberOfReusableHeaderCells>::m_delegate->tableView()
        ->reloadData(false);
  }
}

void InputHomogeneityTableCell::destroyCells() {
  DynamicCellsDataSource<
      EvenOddEditableTextCell,
      k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<
      EvenOddBufferTextCell,
      k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

}  // namespace Inference
