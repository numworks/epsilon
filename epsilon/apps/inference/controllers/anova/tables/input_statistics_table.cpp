#include "input_statistics_table.h"

#include <shared/column_parameter_controller.h>

#include "inference/controllers/anova/input_statistics_controller.h"
#include "inference/controllers/tables/header_titles.h"

using namespace Escher;

namespace Inference::ANOVA {

InputStatisticsTable::InputStatisticsTable(
    Escher::Responder* parentResponder, ANOVATest* test,
    InputStatisticsController* inputStatisticsController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : InputCategoricalTableCell(parentResponder, this, test,
                                scrollViewDelegate),
      m_inputStatisticsController(inputStatisticsController) {}

void InputStatisticsTable::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if ((event.type == ResponderChainEventType::HasBecomeFirst) &&
      (selectedRow() < 0)) {
    selectColumn(0);
  }
  CategoricalTableCell::handleResponderChainEvent(event);
}

size_t InputStatisticsTable::fillColumnName(int column, char* buffer) {
  assert(column >= 0 && column < numberOfColumns());
  OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(column - 1);
  strlcpy(buffer, groupTitle.data(),
          Shared::ColumnParameterController::k_titleBufferSize);
  return groupTitle.length();
}

// The row index refers to the "inner" table
constexpr static const char* HeaderAtRow(int row) {
  assert(row >= 0 && row < 3);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::SampleSizeShort);
    case 1:
      return I18n::translate(I18n::Message::SampleMeanShort);
    case 2:
      return I18n::translate(I18n::Message::SampleSTDShort);
    default:
      OMG::unreachable();
  }
}

void InputStatisticsTable::fillCellForLocation(Escher::HighlightCell* cell,
                                               int column, int row) {
  assert(row >= 0 && row < numberOfRows());
  assert(column >= 0 && column < numberOfColumns());

  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);
  if (type == k_typeOfHeaderCells) {
    if (row == 0) {
      // Column title
      OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(column - 2);
      myCell->setText(groupTitle.data());
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    } else {
      assert(column <= 1);
      if (column == 0) {
        // Row title
        myCell->setText(HeaderAtRow(row - 1));
        myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
      } else {
        // Row symbol
        myCell->setText("df");
        myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
      }
    }
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 2, row - 1);
  }
  myCell->setEven(row % 2 == 0);

  if ((row == 0 && column == numberOfColumns() - 1 &&
       column < k_numberOfInnerColumns - 1)) {
    /* The last header has its title grayed out, except if the column is the
     * last possible column. */
    static_cast<InferenceEvenOddBufferCell*>(cell)->setTextColor(
        Palette::GrayDark);
  }
}

void InputStatisticsTable::fillInnerCellForLocation(Escher::HighlightCell* cell,
                                                    int column, int row) {
  assert(column >= 0 && row >= 0);
  assert(column < innerNumberOfColumns() && row < innerNumberOfRows());
  InferenceEvenOddEditableCell* myCell =
      static_cast<InferenceEvenOddEditableCell*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           column, row, tableModel());
}

CategoricalController* InputStatisticsTable::categoricalController() {
  return m_inputStatisticsController;
}

void InputStatisticsTable::initInnerCell(InferenceEvenOddEditableCell* cell) {
  cell->setParentResponder(&m_selectableTableView);
  cell->editableTextCell()->textField()->setDelegate(this);
}

}  // namespace Inference::ANOVA
