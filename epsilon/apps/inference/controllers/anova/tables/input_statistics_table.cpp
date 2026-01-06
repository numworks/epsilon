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
      GroupTitle(relativeColumn(column));
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
      OMG_UNREACHABLE;
  }
}

constexpr static const char* SymbolAtRow(int row) {
  assert(row >= 0 && row < InputStatisticsTable::k_numberOfInnerRows);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::N);
    case 1:
      return I18n::translate(I18n::Message::MeanSymbol);
    case 2:
      return I18n::translate(I18n::Message::S);
    default:
      OMG_UNREACHABLE;
  }
}

void InputStatisticsTable::fillColumnTitleForLocation(
    Escher::HighlightCell* cell, int innerColumn) {
  assert(innerColumn >= 0 && innerColumn < innerNumberOfColumns());
  headerCellType* myCell = static_cast<headerCellType*>(cell);
  OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(innerColumn);
  myCell->setText(groupTitle.data());
  myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  myCell->setTextColor(KDColorBlack);
  if (innerColumn == innerNumberOfColumns() - 1 &&
      innerColumn < k_numberOfInnerColumns - 1) {
    /* The last header has its title grayed out, except if the column is the
     * last possible column. */
    myCell->setTextColor(Palette::GrayDark);
  }
}
void InputStatisticsTable::fillRowTitleForLocation(Escher::HighlightCell* cell,
                                                   int innerRow) {
  assert(innerRow >= 0 && innerRow < innerNumberOfRows());
  headerCellType* myCell = static_cast<headerCellType*>(cell);
  myCell->setText(HeaderAtRow(innerRow));
  myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
  myCell->setTextColor(KDColorBlack);
}

void InputStatisticsTable::fillRowSymbolForLocation(Escher::HighlightCell* cell,
                                                    int innerRow) {
  assert(innerRow >= 0 && innerRow < innerNumberOfRows());
  headerCellType* myCell = static_cast<headerCellType*>(cell);
  myCell->setText(SymbolAtRow(innerRow));
  myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  myCell->setTextColor(Palette::GrayDark);
}

void InputStatisticsTable::fillInnerCellForLocation(Escher::HighlightCell* cell,
                                                    int innerColumn,
                                                    int innerRow) {
  assert(innerColumn >= 0 && innerRow >= 0);
  assert(innerColumn < innerNumberOfColumns() &&
         innerRow < innerNumberOfRows());
  innerCellType* myCell = static_cast<innerCellType*>(cell);
  fillValueCellForLocation(myCell->editableTextCell()->textField(), myCell,
                           innerColumn, innerRow, tableModel());
}

CategoricalController* InputStatisticsTable::categoricalController() {
  return m_inputStatisticsController;
}

void InputStatisticsTable::initInnerCell(innerCellType* cell) {
  cell->setParentResponder(&m_selectableTableView);
  cell->editableTextCell()->textField()->setDelegate(this);
}

}  // namespace Inference::ANOVA
