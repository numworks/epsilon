#include "list_parameter_controller.h"
#include "list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <apps/i18n.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Sequence {

ListParameterController::ListParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ListController * listController) :
  Shared::ListParameterController(listController, I18n::Message::SequenceColor, I18n::Message::DeleteSequence, this),
  m_typeCell(I18n::Message::SequenceType),
  m_initialRankCell(&m_selectableTableView, inputEventHandlerDelegate, this, I18n::Message::FirstTermIndex),
  m_typeParameterController(this, listController, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)
{
}

const char * ListParameterController::title() {
  return I18n::translate(I18n::Message::SequenceOptions);
}

bool ListParameterController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::Down || event == Ion::Events::Up || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ListParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &floatBody)) {
    return false;
  }
  int index = std::floor(floatBody);
  if (index < 0 || index > Shared::Sequence::maxFirstIndex) {
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
  if (previousSelectedCellY == 1) {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    if (myCell) {
      myCell->setEditing(false);
    }
    Container::activeApp()->setFirstResponder(&m_selectableTableView);
  }
  if (t->selectedRow() == 1) {
    MessageTableCellWithEditableText * myNewCell = (MessageTableCellWithEditableText *)t->selectedCell();
    Container::activeApp()->setFirstResponder(myNewCell);
  }
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  switch (type) {
    case k_typeCellType:
      return &m_typeCell;
    case k_initialRankCellType:
      assert(hasInitialRankRow());
      return &m_initialRankCell;
    default:
      return Shared::ListParameterController::reusableCell(index, type);
  }
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  cell->setHighlighted(index == selectedRow()); // See FIXME in SelectableTableView::reloadData()
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if (cell == &m_typeCell && !m_record.isNull()) {
    assert(typeAtIndex(index) == k_typeCellType);
    m_typeCell.setLayout(sequence()->definitionName());
  }
  if (cell == &m_initialRankCell && !m_record.isNull()) {
    assert(typeAtIndex(index) == k_initialRankCellType);
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
    if (myCell->isEditing()) {
      return;
    }
    char buffer[Shared::Sequence::k_initialRankNumberOfDigits+1];
    Poincare::Integer(sequence()->initialRank()).serialize(buffer, Shared::Sequence::k_initialRankNumberOfDigits+1);
    myCell->setAccessoryText(buffer);
  }
}

int ListParameterController::typeAtIndex(int index) {
  if (index == 0) {
    return k_typeCellType;
  }
  if (index == 1 && hasInitialRankRow()) {
    return k_initialRankCellType;
  }
  return Shared::ListParameterController::typeAtIndex(index);
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  int type = typeAtIndex(rowIndex);
  switch (type) {
    case k_typeCellType: {
      StackViewController * stack = (StackViewController *)(parentResponder());
      m_typeParameterController.setRecord(m_record);
      stack->push(&m_typeParameterController);
      return true;
    }
    case k_enableCellType:
      // Shared::ListParameterController::m_enableCell is selected
      App::app()->localContext()->resetCache();
    default:
      return Shared::ListParameterController::handleEnterOnRow(rowIndex);
  }
}

bool ListParameterController::rightEventIsEnterOnType(int type) {
  return type == k_typeCellType || Shared::ListParameterController::rightEventIsEnterOnType(type);
}

bool ListParameterController::hasInitialRankRow() const {
  return !m_record.isNull() && const_cast<ListParameterController *>(this)->sequence()->type() != Shared::Sequence::Type::Explicit;
}

}
