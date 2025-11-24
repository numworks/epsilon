#include "input_data_table.h"

#include <shared/column_parameter_controller.h>

#include "inference/controllers/anova/input_data_controller.h"
#include "inference/controllers/tables/header_titles.h"

using namespace Escher;

namespace Inference::ANOVA {

InputDataTable::InputDataTable(Escher::Responder* parentResponder,
                               ANOVATest* test,
                               InputDataController* inputDataController,
                               Escher::ScrollViewDelegate* scrollViewDelegate)
    : InputCategoricalTableCell(parentResponder, this, test,
                                scrollViewDelegate),
      m_inputDataController(inputDataController) {}

void InputDataTable::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if ((event.type == ResponderChainEventType::HasBecomeFirst) &&
      (selectedRow() < 0)) {
    selectColumn(0);
  }
  CategoricalTableCell::handleResponderChainEvent(event);
}

size_t InputDataTable::fillColumnName(int column, char* buffer) {
  assert(column >= 0 && column < k_maxNumberOfColumns);
  OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(column);
  strlcpy(buffer, groupTitle.data(),
          Shared::ColumnParameterController::k_titleBufferSize);
  return groupTitle.length();
}

void InputDataTable::fillCellForLocation(Escher::HighlightCell* cell,
                                         int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < numberOfColumns() && row <= numberOfRows());
  int type = typeAtLocation(column, row);
  if (type == k_typeOfHeaderCells) {
    InferenceEvenOddBufferCell* myCell =
        static_cast<InferenceEvenOddBufferCell*>(cell);
    assert(row == 0);
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setFont(KDFont::Size::Small);
    myCell->setEven(true);
    OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(column);
    myCell->setText(groupTitle.data());
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

void InputDataTable::fillInnerCellForLocation(Escher::HighlightCell* cell,
                                              int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < innerNumberOfColumns() && row < innerNumberOfRows());
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

CategoricalController* InputDataTable::categoricalController() {
  return m_inputDataController;
}

void InputDataTable::initInnerCell(InferenceEvenOddEditableCell* cell) {
  cell->setParentResponder(&m_selectableTableView);
  cell->editableTextCell()->textField()->setDelegate(this);
}

}  // namespace Inference::ANOVA
