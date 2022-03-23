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
  // Restore the selected color
  KDColor FunctionColor = function()->color();
  uint8_t cellXPosition = 0;
  // TODO: Improve this if possible
  for (uint8_t i = 0; i < sizeof(Palette::DataColor)/sizeof(Palette::DataColor[0]); i++) {
    if (Palette::DataColor[i] == FunctionColor) {
      cellXPosition = i;
      break;
    }
  }
  assert(Palette::DataColor[cellXPosition] == FunctionColor);
  selectCellAtLocation(0, cellXPosition);
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
