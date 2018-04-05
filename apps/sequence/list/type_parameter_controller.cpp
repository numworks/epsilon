#include "type_parameter_controller.h"
#include "list_controller.h"
#include "../app.h"
#include <assert.h>
#include "../../../poincare/src/layout/baseline_relative_layout.h"
#include "../../../poincare/src/layout/string_layout.h"

using namespace Poincare;

namespace Sequence {

TypeParameterController::TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore, ListController * list, TableCell::Layout cellLayout,
  KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ViewController(parentResponder),
  m_expliciteCell(I18n::Message::Explicit, cellLayout),
  m_singleRecurrenceCell(I18n::Message::SingleRecurrence, cellLayout),
  m_doubleRecurenceCell(I18n::Message::DoubleRecurrence, cellLayout),
  m_expressionLayouts{},
  m_selectableTableView(this, this, 0, 1, topMargin, rightMargin, bottomMargin, leftMargin, this, nullptr, false),
  m_sequenceStore(sequenceStore),
  m_sequence(nullptr),
  m_listController(list)
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

const char * TypeParameterController::title() {
  if (m_sequence) {
    return I18n::translate(I18n::Message::SequenceType);
  }
  return I18n::translate(I18n::Message::ChooseSequenceType);
}

View * TypeParameterController::view() {
  return &m_selectableTableView;
}

void TypeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

void TypeParameterController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

void TypeParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (m_sequence) {
      Sequence::Type sequenceType = (Sequence::Type)selectedRow();
      if (m_sequence->type() != sequenceType) {
        m_listController->selectPreviousNewSequenceCell();
        m_sequence->setType(sequenceType);
        // Invalidate sequence context cache when changing sequence type
        static_cast<App *>(app())->localContext()->resetCache();
        // Reset the first index if the new type is "Explicit"
        if (sequenceType == Sequence::Type::Explicit) {
          m_sequence->setInitialRank(0);
        }
      }
      StackViewController * stack = stackController();
      assert(stack->depth()>2);
      stack->pop();
      stack->pop();
      return true;
    }
    Sequence * newSequence = m_sequenceStore->addEmptyFunction();
    newSequence->setType((Sequence::Type)selectedRow());
    app()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Left && m_sequence) {
    stackController()->pop();
    return true;
  }
  return false;
}

int TypeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * TypeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_expliciteCell, &m_singleRecurrenceCell, &m_doubleRecurenceCell};
  return cells[index];
}

int TypeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate TypeParameterController::cellHeight() {
  if (m_sequence) {
    return Metric::ParameterCellHeight;
  }
  return 50;
}

void TypeParameterController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  const char * nextName = m_sequenceStore->firstAvailableName();
  KDText::FontSize size = KDText::FontSize::Large;
  if (m_sequence) {
    nextName = m_sequence->name();
    size = KDText::FontSize::Small;
  }
  const char * subscripts[3] = {"n", "n+1", "n+2"};
  if (m_expressionLayouts[j]) {
    delete m_expressionLayouts[j];
    m_expressionLayouts[j] = nullptr;
  }
  m_expressionLayouts[j] = new BaselineRelativeLayout(new StringLayout(nextName, 1, size), new StringLayout(subscripts[j], strlen(subscripts[j]), KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  ExpressionTableCellWithPointer * myCell = (ExpressionTableCellWithPointer *)cell;
  myCell->setExpression(m_expressionLayouts[j]);
}

void TypeParameterController::setSequence(Sequence * sequence) {
  m_sequence = sequence;
}

StackViewController * TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
