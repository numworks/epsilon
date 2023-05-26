#include "input_homogeneity_table_cell.h"

#include <shared/column_parameter_controller.h>

#include "input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityTableCell::InputHomogeneityTableCell(
    Escher::Responder *parentResponder, HomogeneityTest *test,
    InputHomogeneityController *inputHomogeneityController)
    : InputCategoricalTableCell(parentResponder, this, test),
      DynamicCellsDataSource<InferenceEvenOddEditableCell,
                             k_homogeneityTableNumberOfReusableInnerCells>(
          this),
      m_inputHomogeneityController(inputHomogeneityController) {}

void InputHomogeneityTableCell::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectColumn(1);
  }
  CategoricalTableCell::didBecomeFirstResponder();
}

void InputHomogeneityTableCell::fillCellForLocation(Escher::HighlightCell *cell,
                                                    int column, int row) {
  HomogeneityTableDataSource::fillCellForLocation(cell, column, row);
  if ((column == 0 && row != 0 && row == numberOfRows() - 1 &&
       row < k_maxNumberOfRows) ||
      (row == 0 && column != 0 && column == numberOfColumns() - 1 &&
       column < k_maxNumberOfColumns)) {
    /* The last column/row is empty and has its title grayed out. */
    InferenceEvenOddBufferCell *typedCell =
        static_cast<InferenceEvenOddBufferCell *>(cell);
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
  InferenceEvenOddEditableCell *myCell =
      static_cast<InferenceEvenOddEditableCell *>(cell);
  willDisplayValueCellAtLocation(myCell->editableTextCell()->textField(),
                                 myCell, column, row, tableModel());
}

void InputHomogeneityTableCell::createCells() {
  /* We could equivalently use
   * DynamicCellsDataSource<InferenceEvenOddEditableCell,
   * k_homogeneityTableNumberOfReusableInnerCells>::m_cells */
  if (DynamicCellsDataSource<
          InferenceEvenOddBufferCell,
          k_homogeneityTableNumberOfReusableHeaderCells>::m_cells == nullptr) {
    DynamicCellsDataSource<InferenceEvenOddBufferCell,
                           k_homogeneityTableNumberOfReusableHeaderCells>::
        createCellsWithOffset(0);
    DynamicCellsDataSource<InferenceEvenOddEditableCell,
                           k_homogeneityTableNumberOfReusableInnerCells>::
        createCellsWithOffset(k_homogeneityTableNumberOfReusableHeaderCells *
                              sizeof(InferenceEvenOddBufferCell));
    /* We could equivalently use
     * DynamicCellsDataSource<InferenceEvenOddEditableCell,
     * k_homogeneityTableNumberOfReusableInnerCells>::m_delegate */
    DynamicCellsDataSource<
        InferenceEvenOddBufferCell,
        k_homogeneityTableNumberOfReusableHeaderCells>::m_delegate->tableView()
        ->reloadData(false);
  }
}

void InputHomogeneityTableCell::destroyCells() {
  DynamicCellsDataSource<
      InferenceEvenOddEditableCell,
      k_homogeneityTableNumberOfReusableInnerCells>::destroyCells();
  DynamicCellsDataSource<
      InferenceEvenOddBufferCell,
      k_homogeneityTableNumberOfReusableHeaderCells>::destroyCells();
}

CategoricalController *InputHomogeneityTableCell::categoricalController() {
  return m_inputHomogeneityController;
}

}  // namespace Inference
