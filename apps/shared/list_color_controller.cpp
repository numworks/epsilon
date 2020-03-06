#include "list_color_controller.h"
#include "function_app.h"
#include <assert.h>
#include <escher.h>
#include <apps/i18n.h>
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Shared {

static I18n::Message colorMessages[] = {
  I18n::Message::Axis,
  I18n::Message::Axis,
  I18n::Message::Axis,
  I18n::Message::Axis,
  I18n::Message::Axis,
  I18n::Message::Axis,
  I18n::Message::Axis,
  I18n::Message::Axis
};

ColorParameterController::ColorParameterController(Responder * parentResponder):
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_record()
{
}

const char * ColorParameterController::title() {
  return "TODO";
}

View * ColorParameterController::view() {
  return &m_selectableTableView;
}

void ColorParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

void ColorParameterController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

void ColorParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool ColorParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
     function()->setColor(Palette::DataColor[selectedRow()]);
     StackViewController * stack = (StackViewController *)(parentResponder());
     stack->pop();
     return true;
  }
  return false;
}

int ColorParameterController::numberOfRows() const {
  return k_totalNumberOfCells;
};

HighlightCell * ColorParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  HighlightCell * cell = &m_cells[index];
  return cell;
}

int ColorParameterController::reusableCellCount() const {
  return k_totalNumberOfCells;
}

KDCoordinate ColorParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void ColorParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(colorMessages[index]);
  myCell->setTextColor(Palette::DataColor[index]);
  myCell->reloadCell();
}

FunctionStore * ColorParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}

ExpiringPointer<Function> ColorParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

}
