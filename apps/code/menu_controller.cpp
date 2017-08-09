#include "menu_controller.h"
#include "../i18n.h"
#include <assert.h>

namespace Code {

MenuController::MenuController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_editorController(nullptr)
{
}

View * MenuController::view() {
  return &m_selectableTableView;
}

void MenuController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    app()->displayModalViewController(&m_editorController, 0.5f, 0.5f);
    return true;
  }
  return false;
}

int MenuController::numberOfRows() {
  return k_totalNumberOfCells;
};


HighlightCell * MenuController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int MenuController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate MenuController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void MenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message titles[k_totalNumberOfCells] = {I18n::Message::EditProgram, I18n::Message::ExecuteProgram};
  myCell->setMessage(titles[index]);
}

}
