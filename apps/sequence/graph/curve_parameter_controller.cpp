#include "curve_parameter_controller.h"

#include <apps/i18n.h>
#include <assert.h>

#include "graph_controller.h"

using namespace Shared;
using namespace Escher;

namespace Sequence {

CurveParameterController::CurveParameterController(
    GraphController* graphController, CobwebController* cobwebController,
    InteractiveCurveViewRange* graphRange, CurveViewCursor* cursor)
    : ExplicitSelectableListViewController(nullptr, nullptr),
      m_goToParameterController(this, graphController, graphRange, cursor),
      m_cobwebController(cobwebController),
      m_graphController(graphController) {
  m_sumCell.label()->setMessage(I18n::Message::TermSum);
  m_cobwebCell.label()->setMessage(I18n::Message::CobwebPlot);
  m_goToCell.label()->setMessage(I18n::Message::Goto);
}

const char* CurveParameterController::title() const {
  return I18n::translate(I18n::Message::SequenceOptions);
}

void CurveParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  m_goToParameterController.setRecord(record);
  m_cobwebController->setRecord(record);
  m_cobwebCell.setVisible(m_cobwebController->isRecordSuitable());
  m_selectableListView.resetSizeAndOffsetMemoization();
}

void CurveParameterController::viewWillAppear() {
  if (selectedRow() < 0 || !selectedCell()->isVisible()) {
    selectRow(0);
  }
  ExplicitSelectableListViewController::viewWillAppear();
  m_selectableListView.reloadData();
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (cell == &m_sumCell && m_sumCell.canBeActivatedByEvent(event)) {
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        false);
    stack->push(m_graphController->termSumController());
    return true;
  }
  if (cell == &m_goToCell && m_goToCell.canBeActivatedByEvent(event)) {
    assert(!m_record.isNull());
    stack->push(&m_goToParameterController);
    return true;
  }
  if (cell == &m_cobwebCell && m_cobwebCell.canBeActivatedByEvent(event)) {
    stack->pop();
    stack->push(m_cobwebController);
    return true;
  }
  return ExplicitSelectableListViewController::handleEvent(event);
}

HighlightCell* CurveParameterController::cell(int row) {
  assert(0 <= row && row < k_numberOfRows);
  HighlightCell* cells[k_numberOfRows] = {&m_sumCell, &m_cobwebCell,
                                          &m_goToCell};
  return cells[row];
}

}  // namespace Sequence
