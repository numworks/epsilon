#include "list_parameter_controller.h"
#include "list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(ListController * listController, SequenceStore * sequenceStore) :
  Shared::ListParameterController(listController, sequenceStore, I18n::Message::SequenceColor, I18n::Message::DeleteSequence),
  m_typeCell(I18n::Message::SequenceType),
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
#if FUNCTION_COLOR_CHOICE
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1)) {
#else
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 0)) {
#endif
    int selectedRowIndex = selectedRow();
    switch (selectedRowIndex) {
#if FUNCTION_COLOR_CHOICE
      case 0:
        return handleEnterOnRow(selectedRowIndex);
      case 1:
#else
      case 0:
#endif
      {
        StackViewController * stack = (StackViewController *)(parentResponder());
        m_typeParameterController.setSequence(m_sequence);
        stack->push(&m_typeParameterController);
        return true;
      }
#if FUNCTION_COLOR_CHOICE
      case 3:
#else
      case 2:
#endif
      if (m_functionStore->numberOfFunctions() > 0) {
        m_functionStore->removeFunction(m_function);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
      default:
        return handleEnterOnRow(selectedRowIndex-1);
    }
  }
  return false;
}

int ListParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * ListParameterController::reusableCell(int index) {
  switch (index) {
    /*case 0:
      return Shared::ListParameterController::reusableCell(index);*/
    case 0://1:
      return &m_typeCell;
    default:
      return Shared::ListParameterController::reusableCell(index-1);
  }
}

int ListParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if (cell == &m_typeCell && m_sequence != nullptr) {
    m_typeCell.setExpression(m_sequence->definitionName());
  }
}

}
