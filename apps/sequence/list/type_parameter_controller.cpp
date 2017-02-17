#include "type_parameter_controller.h"
#include <assert.h>
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"

using namespace Poincare;

namespace Sequence {

TypeParameterController::TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore) :
  ViewController(parentResponder),
  m_expliciteCell(TextExpressionMenuListCell((char*)"Explicite")),
  m_singleRecurrenceCell(TextExpressionMenuListCell((char*)"Recurrence d'ordre 1")),
  m_doubleRecurenceCell(TextExpressionMenuListCell((char*)"Recurrence d'ordre 2")),
  m_selectableTableView(SelectableTableView(this, this)),
  m_sequenceStore(sequenceStore)
{
}

TypeParameterController::~TypeParameterController() {
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    if (m_expressionLayouts[i]) {
      delete m_expressionLayouts[i];
      m_expressionLayouts[i] = nullptr;
    }
  }
}

const char * TypeParameterController::title() const {
  return "Choisir le type de suite";
}

View * TypeParameterController::view() {
  return &m_selectableTableView;
}

void TypeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.dataHasChanged(true);
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    Sequence * newSequence = m_sequenceStore->addEmptyFunction();
    newSequence->setType((Sequence::Type)m_selectableTableView.selectedRow());
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

int TypeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * TypeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_expliciteCell, &m_singleRecurrenceCell, &m_doubleRecurenceCell};
  return cells[index];
}

int TypeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate TypeParameterController::cellHeight() {
  return 50;
}

void TypeParameterController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  const char * nextName = m_sequenceStore->firstAvailableName();
  const char * subscripts[3] = {"n", "n+1", "n+2"};
  if (m_expressionLayouts[j]) {
    delete m_expressionLayouts[j];
    m_expressionLayouts[j] = nullptr;
  }
  m_expressionLayouts[j] = new BaselineRelativeLayout(new StringLayout(nextName, 1), new StringLayout(subscripts[j], strlen(subscripts[j]), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  TextExpressionMenuListCell * myCell = (TextExpressionMenuListCell *)cell;
  myCell->setExpression(m_expressionLayouts[j]);
}

StackViewController * TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
