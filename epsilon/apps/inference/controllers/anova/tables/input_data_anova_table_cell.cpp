#include "input_data_anova_table_cell.h"

#include <poincare/print.h>
#include <shared/column_parameter_controller.h>

#include "inference/controllers/anova/input_data_anova_controller.h"

using namespace Escher;

namespace Inference {

InputDataANOVATableCell::InputDataANOVATableCell(
    Escher::Responder* parentResponder, ANOVATest* test,
    InputDataANOVAController* InputDataANOVAController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : InputCategoricalTableCell(parentResponder, this, test,
                                scrollViewDelegate),
      m_InputDataANOVAController(InputDataANOVAController) {}

void InputDataANOVATableCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if ((event.type == ResponderChainEventType::HasBecomeFirst) &&
      (selectedRow() < 0)) {
    selectColumn(0);
  }
  CategoricalTableCell::handleResponderChainEvent(event);
}

size_t InputDataANOVATableCell::fillColumnName(int column, char* buffer) {
  assert(column >= 0 && column < k_maxNumberOfColumns);
  return Poincare::Print::CustomPrintf(
      buffer, Shared::ColumnParameterController::k_titleBufferSize, "%s %i",
      I18n::translate(I18n::Message::Group), column + 1);
}

void InputDataANOVATableCell::fillCellForLocation(Escher::HighlightCell* cell,
                                                  int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < numberOfColumns() && row <= numberOfRows());
  int type = typeAtLocation(column, row);
  if (type == k_typeOfHeaderCells) {
    InferenceEvenOddBufferCell* myCell =
        static_cast<InferenceEvenOddBufferCell*>(cell);
    char digit;
    assert(row == 0);
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(true);
    assert(column <= '9' - '1');
    digit = '1' + column;
    constexpr int bufferSize = k_headerTranslationBufferSize;
    char txt[bufferSize];
    Poincare::Print::CustomPrintf(txt, bufferSize, "%s %c",
                                  I18n::translate(I18n::Message::Group), digit);
    myCell->setText(txt);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column, row - 1);
  }

  if ((row == 0 && column == numberOfColumns() - 1 &&
       column < k_maxNumberOfColumns - 1)) {
    /* The last header has its title grayed out, except if the column is the
     * last possible column. */
    static_cast<InferenceEvenOddBufferCell*>(cell)->setTextColor(
        Palette::GrayDark);
  }
}

void InputDataANOVATableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < innerNumberOfColumns() && row < innerNumberOfRows());
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

CategoricalController* InputDataANOVATableCell::categoricalController() {
  return m_InputDataANOVAController;
}

void InputDataANOVATableCell::initInnerCell(
    InferenceEvenOddEditableCell* cell) {
  cell->setParentResponder(&m_selectableTableView);
  cell->editableTextCell()->textField()->setDelegate(this);
}

}  // namespace Inference
