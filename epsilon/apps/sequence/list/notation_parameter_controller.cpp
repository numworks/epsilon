#include "notation_parameter_controller.h"

#include <apps/shared/interactive_curve_view_controller.h>
#include <escher/stack_view_controller.h>

#include "../app.h"

using namespace Escher;
using NotationCell =
    Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView>;

namespace Sequence {

void NotationParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectRow(rowForNotation(sequence()->recursiveNotation()));
  m_selectableListView.reloadData();
}

bool NotationParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    sequence()->setRecursiveNotation(notationForRow(selectedRow()));
    // Pop all the way back
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        true);
    return true;
  } else if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  return false;
}

void NotationParameterController::fillCellForRow(HighlightCell* cell, int row) {
  NotationCell* notationCell = static_cast<NotationCell*>(cell);
  assert(row >= 0);
  assert(row < k_numberOfCells);
  // TODO_PAULINE: create and set layout according to row
  notationCell->label()->setLayout(sequence()->nameLayout());
}

void NotationParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

OMG::ExpiringPointer<Shared::Sequence> NotationParameterController::sequence() {
  return Shared::GlobalContextAccessor::SequenceStore().modelForRecord(
      m_record);
}

}  // namespace Sequence
