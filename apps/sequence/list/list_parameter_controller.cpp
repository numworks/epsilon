#include "list_parameter_controller.h"
#include "list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <apps/i18n.h>

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(::InputEventHandlerDelegate * inputEventHandlerDelegate, ListController * listController) :
  Shared::ListParameterController(listController, I18n::Message::SequenceColor, I18n::Message::DeleteSequence, this),
  m_typeCell(I18n::Message::SequenceType),
  m_initialRankCell(&m_selectableTableView, inputEventHandlerDelegate, this, I18n::Message::FirstTermIndex),
  m_typeParameterController(this, listController, TableCell::Layout::HorizontalLeftOverlap, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)
{
}

const char * ListParameterController::title() {
  return I18n::translate(I18n::Message::SequenceOptions);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  bool hasAdditionalRow = hasInitialRankRow();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 0)) {
    int selectedRowIndex = selectedRow();
    if (selectedRowIndex == 0) {
      StackViewController * stack = (StackViewController *)(parentResponder());
      m_typeParameterController.setRecord(m_record);
      stack->push(&m_typeParameterController);
      return true;
    }
    if (selectedRowIndex == 1+hasAdditionalRow || selectedRowIndex == 2+hasAdditionalRow) {
      return handleEnterOnRow(selectedRowIndex-hasAdditionalRow-1);
    }
    if (selectedRowIndex == 3+hasAdditionalRow) {
      App::app()->localContext()->resetCache();
      return handleEnterOnRow(selectedRowIndex-hasAdditionalRow-1);
    }
  }
  return false;
}

bool ListParameterController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::Down || event == Ion::Events::Up || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ListParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  static float maxFirstIndex = std::pow(10.0f, Shared::Sequence::k_initialRankNumberOfDigits) - 1.0f;
  /* -1 to take into account a double recursive sequence, which has
   * SecondIndex = FirstIndex + 1 */
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  int index = std::round(floatBody);
  if (index < 0  || floatBody >= maxFirstIndex) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  sequence()->setInitialRank(index);
  // Invalidate sequence context cache when changing sequence type
  App::app()->localContext()->resetCache();
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  m_selectableTableView.handleEvent(event);
  return true;
}

void ListParameterController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection || (previousSelectedCellX == t->selectedColumn() && previousSelectedCellY == t->selectedRow())) {
    return;
  }
  if (!hasInitialRankRow()) {
    return;
  }
#if FUNCTION_COLOR_CHOICE
  if (previousSelectedCellY == 2) {
#else
  if (previousSelectedCellY == 1) {
#endif
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    if (myCell) {
      myCell->setEditing(false);
    }
    Container::activeApp()->setFirstResponder(&m_selectableTableView);
  }
#if FUNCTION_COLOR_CHOICE
  if (t->selectedRow() == 2) {
#else
  if (t->selectedRow() == 1) {
#endif
    MessageTableCellWithEditableText * myNewCell = (MessageTableCellWithEditableText *)t->selectedCell();
    Container::activeApp()->setFirstResponder(myNewCell);
  }
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  switch (type) {
    /*case 0:
      return Shared::ListParameterController::reusableCell(index);*/
    case 0://1:
      return &m_typeCell;
    case 1:
      if (hasInitialRankRow()) {
        return &m_initialRankCell;
      }
    default:
      return Shared::ListParameterController::reusableCell(index, type-1-hasInitialRankRow());
  }
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  cell->setHighlighted(index == selectedRow()); // See FIXME in SelectableTableView::reloadData()
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if (cell == &m_typeCell && !m_record.isNull()) {
    m_typeCell.setLayout(sequence()->definitionName());
  }
  if (cell == &m_initialRankCell && !m_record.isNull()) {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
    if (myCell->isEditing()) {
      return;
    }
    char buffer[Shared::Sequence::k_initialRankNumberOfDigits+1];
    Poincare::Integer(sequence()->initialRank()).serialize(buffer, Shared::Sequence::k_initialRankNumberOfDigits+1);
    myCell->setAccessoryText(buffer);
  }
}

int ListParameterController::totalNumberOfCells() const {
  if (hasInitialRankRow()) {
    return k_totalNumberOfCell;
  }
  return k_totalNumberOfCell-1;
};

bool ListParameterController::hasInitialRankRow() const {
  return !m_record.isNull() && const_cast<ListParameterController *>(this)->sequence()->type() != Shared::Sequence::Type::Explicit;
}

}
