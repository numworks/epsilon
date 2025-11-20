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
  OMG::String<k_groupTitleBufferSize> groupTitle =
      GroupTitle(column - numberOfHeaderColumns() + 1);
  strlcpy(buffer, groupTitle.data(),
          Shared::ColumnParameterController::k_titleBufferSize);
  return groupTitle.length();
}

// The row index refers to the "inner" table
constexpr static const char* HeaderAtRow(int row) {
  assert(row >= 0 && row < InputStatisticsTable::k_numberOfInnerRows);
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

constexpr static const char* SymbolAtRow(int row) {
  assert(row >= 0 && row <= InputStatisticsTable::k_numberOfInnerRows);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::N);
    case 1:
      return I18n::translate(I18n::Message::MeanSymbol);
    case 2:
      return I18n::translate(I18n::Message::S);
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
  if (type == k_typeOfHeaderCells) {
    fillHeaderCellForLocation(cell, column, row);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - numberOfHeaderColumns(),
                             row - numberOfHeaderRows());
  }
  static_cast<InferenceEvenOddBufferCell*>(cell)->setEven(row % 2 == 0);
}

void InputStatisticsTable::fillHeaderCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);
  if (row < numberOfHeaderRows()) {
    int innerColumnIndex = column - numberOfHeaderColumns();
    // Column title
    OMG::String<k_groupTitleBufferSize> groupTitle =
        GroupTitle(innerColumnIndex);
    myCell->setText(groupTitle.data());
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    myCell->setTextColor(KDColorBlack);
    if (innerColumnIndex == innerNumberOfColumns() - 1 &&
        innerColumnIndex < k_numberOfInnerColumns - 1) {
      /* The last header has its title grayed out, except if the column is the
       * last possible column. */
      myCell->setTextColor(Palette::GrayDark);
    }
  } else {
    assert(column < numberOfHeaderColumns());
    if (column == 0) {
      // Row title
      myCell->setText(HeaderAtRow(row - numberOfHeaderRows()));
      myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
      myCell->setTextColor(KDColorBlack);
    } else {
      assert(column == 1);
      // Row symbol
      myCell->setText(SymbolAtRow(row - numberOfHeaderRows()));
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
      myCell->setTextColor(Palette::GrayDark);
    }
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
