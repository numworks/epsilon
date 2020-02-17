#include "type_parameter_controller.h"
#include "type_helper.h"
#include <apps/i18n.h>
#include "../app.h"
#include <assert.h>

namespace Graph {

TypeParameterController::TypeParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, nullptr),
  m_record()
{
}

void TypeParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(!m_record.isNull());
    Shared::ContinuousFunction::PlotType plotType = static_cast<Shared::ContinuousFunction::PlotType>(selectedRow());
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<Shared::ContinuousFunction> function = myApp->functionStore()->modelForRecord(m_record);
    function->setPlotType(plotType, Poincare::Preferences::sharedPreferences()->angleUnit(), myApp->localContext());
    StackViewController * stack = stackController();
    stack->pop();
    stack->pop();
    return true;
  }
  if (event == Ion::Events::Left && !m_record.isNull()) {
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
  App * myApp = App::app();
  assert(!m_record.isNull());
  Shared::ExpiringPointer<Shared::ContinuousFunction> function = myApp->functionStore()->modelForRecord(m_record);
  int row = static_cast<int>(function->plotType());
  selectCellAtLocation(0, row);
  m_selectableTableView.reloadData();
}

KDCoordinate TypeParameterController::rowHeight(int j) {
  return PlotTypeHelper::Layout(j).layoutSize().height() + 14;
}

void TypeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < k_numberOfTypes);
  MessageTableCellWithExpression * myCell = static_cast<MessageTableCellWithExpression *>(cell);
  myCell->setMessage(PlotTypeHelper::Message(index));
  myCell->setLayout(PlotTypeHelper::Layout(index));
}

MessageTableCellWithExpression * TypeParameterController::reusableCell(int index, int type) {
  assert(0 <= index && index < reusableCellCount(type));
  return &m_cells[index];
}

StackViewController * TypeParameterController::stackController() const {
  return static_cast<StackViewController *>(parentResponder());
}

}
