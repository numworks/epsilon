#include "list_parameter_controller.h"
#include "list_controller.h"
#include "../app.h"
#include "../../apps_container.h"

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(ListController * listController, SequenceStore * sequenceStore) :
  Shared::ListParameterController(listController, sequenceStore, I18n::Message::SequenceColor, I18n::Message::DeleteSequence, this),
  m_typeCell(I18n::Message::SequenceType),
  m_initialRankCell(&m_selectableTableView, this, m_draftTextBuffer, I18n::Message::FirstTermIndex),
  m_typeParameterController(this, sequenceStore, listController, TableCell::Layout::Horizontal, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin),
  m_sequence(nullptr)
{
}

const char * ListParameterController::title() {
  return I18n::translate(I18n::Message::SequenceOptions);
}

void ListParameterController::setFunction(Shared::Function * function) {
  Shared::ListParameterController::setFunction(function);
  m_sequence = (Sequence *)function;
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  bool hasAdditionalRow = hasInitialRankRow();
#if FUNCTION_COLOR_CHOICE
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1)) {
#else
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 0)) {
#endif
    int selectedRowIndex = selectedRow();
#if FUNCTION_COLOR_CHOICE
    if (selectedRowIndex == 0) {
      return handleEnterOnRow(selectedRowIndex);
    }
    if (selectedRowIndex == 1) {
#else
    if (selectedRowIndex == 0) {
#endif
      StackViewController * stack = (StackViewController *)(parentResponder());
      m_typeParameterController.setSequence(m_sequence);
      stack->push(&m_typeParameterController);
      return true;
    }
#if FUNCTION_COLOR_CHOICE
    if (selectedRowIndex == 2+hasAdditionalRow) {

#else
    if (selectedRowIndex == 1+hasAdditionalRow) {
#endif
      return handleEnterOnRow(selectedRowIndex-hasAdditionalRow-1);
    }
#if FUNCTION_COLOR_CHOICE
    if (selectedRowIndex == 3+hasAdditionalRow) {
#else
    if (selectedRowIndex == 2+hasAdditionalRow) {
#endif
      if (m_functionStore->numberOfFunctions() > 0) {
        m_functionStore->removeFunction(m_function);
        static_cast<App *>(app())->localContext()->resetCache();
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
    }
  }
  return false;
}

int ListParameterController::numberOfRows() {
  if (hasInitialRankRow()) {
    return k_totalNumberOfCell;
  }
  return k_totalNumberOfCell-1;
};

HighlightCell * ListParameterController::reusableCell(int index) {
  switch (index) {
    /*case 0:
      return Shared::ListParameterController::reusableCell(index);*/
    case 0://1:
      return &m_typeCell;
    case 1:
      if (hasInitialRankRow()) {
        return &m_initialRankCell;
      }
    default:
      return Shared::ListParameterController::reusableCell(index-1-hasInitialRankRow());
  }
}

int ListParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  cell->setHighlighted(index == selectedRow()); // See FIXME in SelectableTableView::reloadData()
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if (cell == &m_typeCell && m_sequence != nullptr) {
    m_typeCell.setExpression(m_sequence->definitionName());
  }
  if (cell == &m_initialRankCell && m_sequence != nullptr) {
    MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
    if (myCell->isEditing()) {
      return;
    }
    char buffer[Sequence::k_initialRankNumberOfDigits+1];
    Integer(m_sequence->initialRank()).writeTextInBuffer(buffer, Sequence::k_initialRankNumberOfDigits+1);
    myCell->setAccessoryText(buffer);
  }
}

bool ListParameterController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::Down || event == Ion::Events::Up || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ListParameterController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  double floatBody = Expression::approximateToScalar<double>(text, *globalContext);
  int index = std::round(floatBody);
  if (std::isnan(floatBody) || std::isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (index < 0) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_sequence->setInitialRank(index);
  // Invalidate sequence context cache when changing sequence type
  static_cast<App *>(app())->localContext()->resetCache();
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  m_selectableTableView.handleEvent(event);
  return true;
}

void ListParameterController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == t->selectedColumn() && previousSelectedCellY == t->selectedRow()) {
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
    myCell->setEditing(false);
    app()->setFirstResponder(&m_selectableTableView);
  }
#if FUNCTION_COLOR_CHOICE
  if (t->selectedRow() == 2) {
#else
  if (t->selectedRow() == 1) {
#endif
    MessageTableCellWithEditableText * myNewCell = (MessageTableCellWithEditableText *)t->selectedCell();
    app()->setFirstResponder(myNewCell);
  }
}

TextFieldDelegateApp * ListParameterController::textFieldDelegateApp() {
  return (TextFieldDelegateApp *)app();
}

bool ListParameterController::hasInitialRankRow() {
  return m_sequence && m_sequence->type() != Sequence::Type::Explicit;
}

}
