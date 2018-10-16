#include "storage_function_curve_parameter_controller.h"
#include <assert.h>

namespace Shared {

StorageFunctionCurveParameterController::StorageFunctionCurveParameterController(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  ViewController(nullptr),
  m_goToCell(I18n::Message::Goto),
  m_selectableTableView(this, this, this),
  m_record()
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
  if (m_record.isNull()) {
    return false;
  }
  goToParameterController()->setRecord(m_record);
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->push(goToParameterController());
  return true;
}

KDCoordinate StorageFunctionCurveParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void StorageFunctionCurveParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

}
