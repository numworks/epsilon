#include "input_anova_table_cell.h"

#include <poincare/print.h>
#include <shared/column_parameter_controller.h>

#include "inference/controllers/input_anova_controller.h"
#include "inference/controllers/tables/anova_table_dimensions.h"

using namespace Escher;

namespace Inference {

InputANOVATableCell::InputANOVATableCell(
    Escher::Responder* parentResponder, ANOVATest* test,
    InputANOVAController* inputANOVAController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : InputCategoricalTableCell(parentResponder, this, test,
                                scrollViewDelegate),
      m_inputANOVAController(inputANOVAController),
      m_headerPrefix(I18n::Message::Group) {}

void InputANOVATableCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if ((event.type == ResponderChainEventType::HasBecomeFirst) &&
      (selectedRow() < 0)) {
    selectColumn(0);
  }
  CategoricalTableCell::handleResponderChainEvent(event);
}

size_t InputANOVATableCell::fillColumnName(int column, char* buffer) {
  assert(column >= 0 && column < k_maxNumberOfColumns);
  return Poincare::Print::CustomPrintf(
      buffer, Shared::ColumnParameterController::k_titleBufferSize, "%s %i",
      I18n::translate(I18n::Message::Group), column);
}

void InputANOVATableCell::fillCellForLocation(Escher::HighlightCell* cell,
                                              int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < k_maxNumberOfColumns && row <= k_maxNumberOfRows);
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
                                  I18n::translate(m_headerPrefix), digit);
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

void InputANOVATableCell::fillInnerCellForLocation(Escher::HighlightCell* cell,
                                                   int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < k_maxNumberOfColumns && row < k_maxNumberOfInnerRows);
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

CategoricalController* InputANOVATableCell::categoricalController() {
  return m_inputANOVAController;
}

void InputANOVATableCell::initCellType2(InferenceEvenOddEditableCell* cell) {
  cell->setParentResponder(&m_selectableTableView);
  cell->editableTextCell()->textField()->setDelegate(this);
}

}  // namespace Inference
