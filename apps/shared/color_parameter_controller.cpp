#include "color_parameter_controller.h"

#include "function_app.h"
#include "../apps_container.h"
#include <assert.h>

namespace Shared {

ColorParameterController::ColorParameterController(Responder * parentResponder, I18n::Message title) :
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_record(),
  m_title(title)
{}

void ColorParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectCellAtLocation(0,0);
  m_selectableTableView.reloadData();
}

void ColorParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool ColorParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    handleEnterOnRow(selectedRow());
    stack->pop();
    return true;
  }
  return false;
}

KDCoordinate ColorParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

HighlightCell * ColorParameterController::reusableCell(int index) {
  assert(index < numberOfRows());
  return &m_cells[index];
}

void ColorParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithColor * myCell = (MessageTableCellWithColor *)cell;
  myCell->setColor(index);
  myCell->setMessageFont(KDFont::LargeFont);
  cell->reloadCell();
}

bool ColorParameterController::handleEnterOnRow(int rowIndex) {
  function()->setColor(Palette::DataColor[rowIndex]);
  return true;
}

void ColorParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectCellAtLocation(0, 0);
}

ExpiringPointer<Function> ColorParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore * ColorParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}


}

