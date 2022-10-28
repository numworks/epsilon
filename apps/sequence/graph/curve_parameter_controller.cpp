#include "curve_parameter_controller.h"
#include "apps/shared/function_curve_parameter_controller.h"
#include "graph_controller.h"
#include <assert.h>
#include <apps/i18n.h>

using namespace Shared;
using namespace Escher;

namespace Sequence {

CurveParameterController::CurveParameterController(InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController, CobwebController * cobwebController, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  FunctionCurveParameterController(),
  m_goToParameterController(this, inputEventHandlerDelegate, graphController, graphRange, cursor),
  m_sumCell(I18n::Message::TermSum),
  m_cobwebCell(I18n::Message::CobwebPlot),
  m_cobwebController(cobwebController),
  m_graphController(graphController)
{
}

const char * CurveParameterController::title() {
  return I18n::translate(I18n::Message::SequenceOptions);
}

void CurveParameterController::viewWillAppear() {
  // We need to set the record early to know if we should display the cobweb entry
  m_cobwebController->setRecord(m_record);
  FunctionCurveParameterController::viewWillAppear();
  m_selectableTableView.reloadData();
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1 + hasCobweb())) {
    switch (selectedRow()) {
      case 0:
      {
        StackViewController * stack = (StackViewController *)parentResponder();
        stack->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, false);
        stack->push(m_graphController->termSumController());
        return true;
      }
      case 1:
        if (hasCobweb()) {
          StackViewController * stack = (StackViewController *)parentResponder();
          stack->pop();
          stack->push(m_cobwebController);
          return true;
        }
        return handleGotoSelection();
      case 2:
        return handleGotoSelection();
      default:
        return false;
    }
  }
  return FunctionCurveParameterController::handleEvent(event);
}

int CurveParameterController::numberOfRows() const {
  return k_totalNumberOfCells - !hasCobweb();
};

HighlightCell * CurveParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  if (!hasCobweb() && index == 1) {
    index = 2;
  }
  HighlightCell * cells[k_totalNumberOfCells] = {&m_sumCell, &m_cobwebCell, &m_goToCell};
  return cells[index];
}

GoToParameterController * CurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

bool CurveParameterController::hasCobweb() const {
  return m_cobwebController->isRecordSuitable();
}

}
