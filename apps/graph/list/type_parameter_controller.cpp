#include "type_parameter_controller.h"
#include <poincare/layout_helper.h>
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>
// TODO Hugo : Delete type_helper
using namespace Escher;

namespace Graph {

TypeParameterController::TypeParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder),
  m_record()
{
}

void TypeParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    // TODO Hugo : Copy paste relevant content
    return true;
  }
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

const char * TypeParameterController::title() {
  return I18n::translate(I18n::Message::CurveType);
}

void TypeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  assert(!m_record.isNull());
  selectCellAtLocation(0, 0);
  resetMemoization();
  m_selectableTableView.reloadData();
}

KDCoordinate TypeParameterController::nonMemoizedRowHeight(int j) {
  MessageTableCellWithExpression tempCell;
  return heightForCellAtIndex(&tempCell, j);
}

void TypeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < k_numberOfDataPoints);
  MessageTableCellWithExpression * myCell = static_cast<MessageTableCellWithExpression *>(cell);
  // TODO Hugo : Create and retrive the correct messages
  myCell->setMessage(I18n::Message::CartesianType);
  myCell->setLayout(Poincare::LayoutHelper::String("3.1415", strlen("3.1415")));
}

MessageTableCellWithExpression * TypeParameterController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  return &m_cells[index];
}

StackViewController * TypeParameterController::stackController() const {
  return static_cast<StackViewController *>(parentResponder());
}

}
