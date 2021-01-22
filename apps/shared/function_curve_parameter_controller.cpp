#include "function_curve_parameter_controller.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

FunctionCurveParameterController::FunctionCurveParameterController() :
  ViewController(nullptr),
  m_goToCell(I18n::Message::Goto),
  m_selectableTableView(this, this, this),
  m_record()
{
}

void FunctionCurveParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool FunctionCurveParameterController::handleGotoSelection() {
  if (m_record.isNull()) {
    return false;
  }
  goToParameterController()->setRecord(m_record);
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->push(goToParameterController());
  return true;
}

KDCoordinate FunctionCurveParameterController::nonMemoizedRowHeight(int j) {
  Escher::HighlightCell * cell = reusableCell(j, typeAtLocation(0, j));
  prepareCellForHeightCalculation(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
}

}
