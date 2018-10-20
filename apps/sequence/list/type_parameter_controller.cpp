#include "type_parameter_controller.h"
#include "list_controller.h"
#include "../app.h"
#include <assert.h>
#include <poincare/layout_helper.h>
#include <poincare/char_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Sequence {

TypeParameterController::TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore, ListController * list, TableCell::Layout cellLayout,
  KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  ViewController(parentResponder),
  m_expliciteCell(I18n::Message::Explicit, cellLayout),
  m_singleRecurrenceCell(I18n::Message::SingleRecurrence, cellLayout),
  m_doubleRecurenceCell(I18n::Message::DoubleRecurrence, cellLayout),
  m_layouts{},
  m_selectableTableView(this),
  m_sequenceStore(sequenceStore),
  m_sequence(nullptr),
  m_listController(list)
{
  m_selectableTableView.setMargins(topMargin, rightMargin, bottomMargin, leftMargin);
  m_selectableTableView.setShowsIndicators(false);
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
    Sequence * newSequence = static_cast<Sequence *>(m_sequenceStore->addEmptyModel());
    newSequence->setType((Sequence::Type)selectedRow());
    app()->dismissModalViewController();
    m_listController->editExpression(newSequence, 0, Ion::Events::OK);
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
  const KDFont * font = KDFont::LargeFont;
  if (m_sequence) {
    nextName = m_sequence->name();
    font = KDFont::SmallFont;
  }
  const char * subscripts[3] = {"n", "n+1", "n+2"};
  m_layouts[j] = HorizontalLayout(
        CharLayout(nextName[0], font),
        VerticalOffsetLayout(LayoutHelper::String(subscripts[j], strlen(subscripts[j]), font), VerticalOffsetLayoutNode::Type::Subscript)
      );
  ExpressionTableCellWithPointer * myCell = (ExpressionTableCellWithPointer *)cell;
  myCell->setLayout(m_layouts[j]);
}

void TypeParameterController::setSequence(Sequence * sequence) {
  m_sequence = sequence;
}

StackViewController * TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
