#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include <assert.h>
#include <apps/i18n.h>

using namespace Shared;
using namespace Escher;

namespace Sequence {

CurveParameterController::CurveParameterController(InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController, CobwebController * cobwebController, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  ExplicitSelectableListViewController(nullptr, nullptr),
  m_goToParameterController(this, inputEventHandlerDelegate, graphController, graphRange, cursor),
  m_goToCell(I18n::Message::Goto),
  m_sumCell(I18n::Message::TermSum),
  m_cobwebCell(I18n::Message::CobwebPlot),
  m_cobwebController(cobwebController),
  m_graphController(graphController)
{
}

const char * CurveParameterController::title() {
  return I18n::translate(I18n::Message::SequenceOptions);
}

void CurveParameterController::setRecord(Ion::Storage::Record record) {
  WithRecord::setRecord(record);
  m_goToParameterController.setRecord(record);
  m_cobwebController->setRecord(record);
  m_cobwebCell.setVisible(m_cobwebController->isRecordSuitable());
  resetMemoization();
}

void CurveParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void CurveParameterController::viewWillAppear() {
  if (selectedRow() < 0 || !selectedCell()->isVisible()) {
    selectCellAtLocation(0, 0);
  }
  ExplicitSelectableListViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell * cell = selectedCell();
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (cell == &m_sumCell && m_sumCell.ShouldEnterOnEvent(event)) {
    stack->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, false);
    stack->push(m_graphController->termSumController());
    return true;
  }
  if (cell == &m_goToCell && m_goToCell.ShouldEnterOnEvent(event)) {
    assert(!m_record.isNull());
    stack->push(&m_goToParameterController);
    return true;
  }
  if (cell == &m_cobwebCell && m_cobwebCell.ShouldEnterOnEvent(event)) {
    stack->pop();
    stack->push(m_cobwebController);
    return true;
  }
  return ExplicitSelectableListViewController::handleEvent(event);
}

HighlightCell * CurveParameterController::cell(int index) {
  assert(0 <= index && index < k_numberOfRows);
  HighlightCell * cells[k_numberOfRows] = {&m_sumCell, &m_cobwebCell, &m_goToCell};
  return cells[index];
}

}
