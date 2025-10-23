#include "input_homogeneity_table_cell.h"

#include <omg/print.h>
#include <shared/column_parameter_controller.h>

#include "homogeneity_data_source.h"
#include "inference/controllers/chi_square/input_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

InputHomogeneityTableCell::InputHomogeneityTableCell(
    Escher::Responder* parentResponder, HomogeneityTest* test,
    InputHomogeneityController* inputHomogeneityController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : InputCategoricalTableCell(parentResponder, this, test,
                                scrollViewDelegate),
      InputHomogeneityInnerCellsDataSource(this),
      m_inputHomogeneityController(inputHomogeneityController) {}

void InputHomogeneityTableCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0) {
      selectColumn(1);
    }
    CategoricalTableCell::handleResponderChainEvent(event);
  } else {
    CategoricalTableCell::handleResponderChainEvent(event);
  }
}

void InputHomogeneityTableCell::fillCellForLocation(Escher::HighlightCell* cell,
                                                    int column, int row) {
  HomogeneityTableDataSource::fillCellForLocation(cell, column, row);
  if ((column == 0 && row != 0 && row == numberOfRows() - 1 &&
       row < k_maxNumberOfRows) ||
      (row == 0 && column != 0 && column == numberOfColumns() - 1 &&
       column < k_maxNumberOfColumns)) {
    /* The last column/row is empty and has its title grayed out. */
    static_cast<InferenceEvenOddBufferCell*>(cell)->setTextColor(
        Palette::GrayDark);
  }
}

size_t InputHomogeneityTableCell::fillColumnName(int column, char* buffer) {
  constexpr size_t bufferSize =
      Shared::ColumnParameterController::k_titleBufferSize;
  size_t length =
      strlcpy(buffer, I18n::translate(I18n::Message::Group), bufferSize);
  buffer[length++] = ' ';
  length += OMG::Print::IntLeft(column, buffer + length, bufferSize - length);
  buffer[length] = 0;
  assert(length < Shared::ColumnParameterController::k_titleBufferSize);
  return length;
}

void InputHomogeneityTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

void InputHomogeneityTableCell::createCells() {
  if (HomogeneityHeaderCellsDataSource::m_cells == nullptr) {
    HomogeneityHeaderCellsDataSource::createCellsWithOffset(0);
    InputHomogeneityInnerCellsDataSource::createCellsWithOffset(
        HomogeneityTableDimensions::k_numberOfHeaderReusableCells *
        sizeof(InferenceEvenOddBufferCell));
  }
}

void InputHomogeneityTableCell::destroyCells() {
  InputHomogeneityInnerCellsDataSource::destroyCells();
  HomogeneityHeaderCellsDataSource::destroyCells();
}

CategoricalController* InputHomogeneityTableCell::categoricalController() {
  return m_inputHomogeneityController;
}

}  // namespace Inference
