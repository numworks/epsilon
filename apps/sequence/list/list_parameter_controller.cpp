#include "list_parameter_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(Responder * parentResponder, SequenceStore * sequenceStore) :
  Shared::ListParameterController(parentResponder, sequenceStore),
  m_typeCell(PointerTableCellWithChevronAndExpression((char *)"Type de suite")),
  m_typeParameterController(TypeParameterController(this, sequenceStore, TableCell::Layout::Horizontal, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin))
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
  if (event == Ion::Events::OK && m_selectableTableView.selectedRow() == 3) {
    StackViewController * stack = (StackViewController *)(parentResponder());
    m_typeParameterController.setSequence(m_sequence);
    stack->push(&m_typeParameterController);
    return true;
  }
  if (event == Ion::Events::OK && m_selectableTableView.selectedRow() == 2) {
    if (m_functionStore->numberOfFunctions() > 0) {
      m_functionStore->removeFunction(m_function);
      StackViewController * stack = (StackViewController *)(parentResponder());
      stack->pop();
      return true;
    }
  }
  return Shared::ListParameterController::handleEvent(event);
}

int ListParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * ListParameterController::reusableCell(int index) {
  if (index == 3) {
    return &m_typeCell;
  }
  return Shared::ListParameterController::reusableCell(index);
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
