#include "function_curve_parameter_controller.h"
#include <assert.h>

namespace Shared {

FunctionCurveParameterController::FunctionCurveParameterController(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  ViewController(nullptr),
  m_goToCell(I18n::Message::Goto),
  m_selectableTableView(this, this, this),
  m_record()
{
}

View * FunctionCurveParameterController::view() {
  return &m_selectableTableView;
}

void FunctionCurveParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
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

KDCoordinate FunctionCurveParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void FunctionCurveParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

}
