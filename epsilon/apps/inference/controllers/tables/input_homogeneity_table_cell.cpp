#include "input_homogeneity_table_cell.h"

#include <poincare/print.h>
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
  return Poincare::Print::CustomPrintf(
      buffer, Shared::ColumnParameterController::k_titleBufferSize, "%s %i",
      I18n::translate(I18n::Message::Group), column);
}

void InputHomogeneityTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

void InputHomogeneityTableCell::createCells() {
  if (DynamicCellsDataSource<InferenceEvenOddBufferCell>::m_cells == nullptr) {
    DynamicCellsDataSource<InferenceEvenOddBufferCell>::createCellsWithOffset(
        HomogeneityTableDimensions::k_numberOfHeaderReusableCells, 0);
    DynamicCellsDataSource<InferenceEvenOddEditableCell>::createCellsWithOffset(
        HomogeneityTableDimensions::k_numberOfInnerReusableCells,
        HomogeneityTableDimensions::k_numberOfHeaderReusableCells *
            sizeof(InferenceEvenOddBufferCell));
  }
}

void InputHomogeneityTableCell::destroyCells() {
  DynamicCellsDataSource<InferenceEvenOddEditableCell>::destroyCells();
  DynamicCellsDataSource<InferenceEvenOddBufferCell>::destroyCells();
}

CategoricalController* InputHomogeneityTableCell::categoricalController() {
  return m_inputHomogeneityController;
}

void InputHomogeneityTableCell::initCell(Escher::HighlightCell* cell,
                                         int index) {
  InferenceEvenOddEditableCell* c =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  c->setParentResponder(&m_selectableTableView);
  c->editableTextCell()->textField()->setDelegate(this);
}

}  // namespace Inference
