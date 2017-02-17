#include "list_parameter_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(Responder * parentResponder, SequenceStore * sequenceStore) :
  Shared::ListParameterController(parentResponder, sequenceStore),
  m_typeCell(PointerTableCellWithChevronAndExpression((char *)"Type de suite")),
  m_typeParameterController(TypeParameterController(this, sequenceStore, TableCell::Layout::Horizontal, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin))
{
}

const char * ListParameterController::title() const {
  return "Options de la suite";
}

void ListParameterController::setFunction(Shared::Function * function) {
  Shared::ListParameterController::setFunction(function);
  m_sequence = (Sequence *)function;
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    int selectedRowIndex = m_selectableTableView.selectedRow();
    switch (selectedRowIndex) {
      case 0:
        return handleEnterOnRow(selectedRowIndex);
      case 1:
      {
        StackViewController * stack = (StackViewController *)(parentResponder());
        m_typeParameterController.setSequence(m_sequence);
        stack->push(&m_typeParameterController);
        return true;
      }
      case 3:
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
    case 0:
      return Shared::ListParameterController::reusableCell(index);
    case 1:
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
