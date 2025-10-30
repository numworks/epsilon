#include "input_anova_table_cell.h"

#include <omg/print.h>
#include <shared/column_parameter_controller.h>

#include "anova_data_source.h"
#include "inference/controllers/input_anova_controller.h"

using namespace Escher;

namespace Inference {

InputANOVATableCell::InputANOVATableCell(
    Escher::Responder* parentResponder, ANOVATest* test,
    InputANOVAController* inputANOVAController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : InputCategoricalTableCell(parentResponder, this, test,
                                scrollViewDelegate),
      InputANOVAInnerCellsDataSource(this),
      m_inputANOVAController(inputANOVAController) {}

void InputANOVATableCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if ((event.type == ResponderChainEventType::HasBecomeFirst) &&
      (selectedRow() < 0)) {
    selectColumn(0);
  }
  CategoricalTableCell::handleResponderChainEvent(event);
}

void InputANOVATableCell::fillCellForLocation(Escher::HighlightCell* cell,
                                              int column, int row) {
  assert(column >= 0 && row >= 0);
  ANOVATableDataSource::fillCellForLocation(cell, column, row);
  if ((row == 0 && column == numberOfColumns() - 1 &&
       column < k_maxNumberOfColumns - 1)) {
    /* The last header has its title grayed out, except if the column is the
     * last possible column. */
    static_cast<InferenceEvenOddBufferCell*>(cell)->setTextColor(
        Palette::GrayDark);
  }
}

size_t InputANOVATableCell::fillColumnName(int column, char* buffer) {
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

void InputANOVATableCell::fillInnerCellForLocation(Escher::HighlightCell* cell,
                                                   int column, int row) {
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

void InputANOVATableCell::createCells() {
  if (ANOVAHeaderCellsDataSource::m_cells == nullptr) {
    ANOVAHeaderCellsDataSource::createCellsWithOffset(0);
    InputANOVAInnerCellsDataSource::createCellsWithOffset(
        ANOVATableDimensions::k_numberOfHeaderReusableCells *
        sizeof(InferenceEvenOddBufferCell));
  }
}

void InputANOVATableCell::destroyCells() {
  InputANOVAInnerCellsDataSource::destroyCells();
  ANOVAHeaderCellsDataSource::destroyCells();
}

CategoricalController* InputANOVATableCell::categoricalController() {
  return m_inputANOVAController;
}

}  // namespace Inference
