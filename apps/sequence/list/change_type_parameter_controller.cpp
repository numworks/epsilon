#include "change_type_parameter_controller.h"
#include <assert.h>
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"

using namespace Poincare;

namespace Sequence {

ChangeTypeParameterController::ChangeTypeParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_sequence(nullptr)
{
}

ChangeTypeParameterController::~ChangeTypeParameterController() {
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    if (m_expressionLayouts[i]) {
      delete m_expressionLayouts[i];
      m_expressionLayouts[i] = nullptr;
    }
  }
}

const char * ChangeTypeParameterController::title() const {
  return "Type de suite";
}

View * ChangeTypeParameterController::view() {
  return &m_selectableTableView;
}

void ChangeTypeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool ChangeTypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK && m_sequence != nullptr) {
    m_sequence->setType((Sequence::Type)m_selectableTableView.selectedRow());
    StackViewController * stack = stackController();
    stack->pop();
    return true;
  }
  return false;
}

int ChangeTypeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * ChangeTypeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_expliciteCell, &m_singleRecurrenceCell, &m_doubleRecurenceCell};
  return cells[index];
}

int ChangeTypeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ChangeTypeParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void ChangeTypeParameterController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  const char * name = m_sequence->name();
  const char * subscripts[3] = {"n", "n+1", "n+2"};
  if (m_expressionLayouts[j]) {
    delete m_expressionLayouts[j];
    m_expressionLayouts[j] = nullptr;
  }
  m_expressionLayouts[j] = new BaselineRelativeLayout(new StringLayout(name, 1), new StringLayout(subscripts[j], strlen(subscripts[j]), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  TextExpressionMenuListCell * myCell = (TextExpressionMenuListCell *)cell;
  myCell->setExpression(m_expressionLayouts[j]);
}

void ChangeTypeParameterController::setSequence(Sequence * sequence) {
  m_sequence = sequence;
}

StackViewController * ChangeTypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
