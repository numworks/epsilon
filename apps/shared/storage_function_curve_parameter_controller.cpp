#include "storage_function_curve_parameter_controller.h"
#include <assert.h>

namespace Shared {

StorageFunctionCurveParameterController::StorageFunctionCurveParameterController(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  ViewController(nullptr),
  m_goToCell(I18n::Message::Goto),
  m_selectableTableView(this, this, this),
  m_function(nullptr)
{
}

View * StorageFunctionCurveParameterController::view() {
  return &m_selectableTableView;
}

void StorageFunctionCurveParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool StorageFunctionCurveParameterController::handleGotoSelection() {
  if (m_function == nullptr) {
    return false;
  }
  goToParameterController()->setFunction(m_function);
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->push(goToParameterController());
  return true;
}

KDCoordinate StorageFunctionCurveParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void StorageFunctionCurveParameterController::setFunction(StorageFunction * function) {
  m_function = function;
}

}
