#include "list_parameter_controller.h"
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"

using namespace Poincare;
using namespace Shared;

namespace Sequence {

ListParameterController::ListParameterController(Responder * parentResponder, SequenceStore * sequenceStore) :
  Shared::ListParameterController(parentResponder, sequenceStore),
  m_typeCell(ChevronExpressionMenuListCell((char *)"Type de suite")),
  m_changeTypeParameterController(this)
{
}

ListParameterController::~ListParameterController() {
  if (m_typeLayout) {
    delete m_typeLayout;
    m_typeLayout = nullptr;
  }
}

const char * ListParameterController::title() const {
  return "Options de la suite";
}

void ListParameterController::setSequence(Sequence * sequence) {
  setFunction(sequence);
  m_sequence = sequence;
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && m_selectableTableView.selectedRow() == 3) {
    StackViewController * stack = (StackViewController *)(parentResponder());
    m_changeTypeParameterController.setSequence(m_sequence);
    stack->push(&m_changeTypeParameterController);
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

TableViewCell * ListParameterController::reusableCell(int index) {
  if (index == 3) {
    return &m_typeCell;
  }
  return Shared::ListParameterController::reusableCell(index);
}

int ListParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void ListParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if (cell == &m_typeCell && m_sequence != nullptr) {
    if (m_typeLayout != nullptr) {
      delete m_typeLayout;
      m_typeLayout = nullptr;
    }
    if (m_sequence->type() == Sequence::Type::Explicite) {
      m_typeLayout = new BaselineRelativeLayout(new StringLayout(m_sequence->name(), 1), new StringLayout("n", 1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    if (m_sequence->type() == Sequence::Type::SingleRecurrence) {
      m_typeLayout = new BaselineRelativeLayout(new StringLayout(m_sequence->name(), 1), new StringLayout("n+1", 3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    if (m_sequence->type() == Sequence::Type::DoubleRecurrence) {
      m_typeLayout = new BaselineRelativeLayout(new StringLayout(m_sequence->name(), 1), new StringLayout("n+2", 3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
    }
    m_typeCell.setExpression(m_typeLayout);
  }
}

}
