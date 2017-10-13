#include "menu_controller.h"
#include "../i18n.h"
#include <assert.h>

namespace Code {

MenuController::MenuController(Responder * parentResponder, Program * program, ButtonRowController * footer) :
  ViewController(parentResponder),
  ButtonRowDelegate(nullptr, footer),
  m_addNewProgramCell(I18n::Message::AddScript),
  m_editorController(program),
  m_consoleController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_consoleButton(this, I18n::Message::Console, Invocation([](void * context, void * sender) {
    MenuController * menu = (MenuController *)context;
    menu->app()->displayModalViewController(menu->consoleController(), 0.5f, 0.5f);
  }, this))
{
  m_stackViewController = (StackViewController *) (parentResponder->parentResponder()); // TODO: Dirty?
}

ConsoleController * MenuController::consoleController() {
  return &m_consoleController;
}

View * MenuController::view() {
  return &m_selectableTableView;
}

void MenuController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool MenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    footer()->setSelectedButton(0);
    return true;
  } else if (event == Ion::Events::Up) {
    footer()->setSelectedButton(-1);
    m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
    app()->setFirstResponder(&m_selectableTableView);
    return true;
  }
  ViewController * vc[2] = {&m_editorController, &m_consoleController};
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    app()->displayModalViewController(vc[selectedRow()], 0.5f, 0.5f);
    return true;
  }
  return false;
}

int MenuController::numberOfRows() {
  return k_totalNumberOfCells + 1;
};

KDCoordinate MenuController::cellHeight() {
  return Metric::ParameterCellHeight;
}

HighlightCell * MenuController::reusableCell(int index) {
  assert(index >= 0);
  if (index < k_totalNumberOfCells) {
    return &m_cells[index];
  }
  assert(index == k_totalNumberOfCells);
  return &m_addNewProgramCell;
}

int MenuController::reusableCellCount() {
  return k_totalNumberOfCells + 1;
}

void MenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index < k_totalNumberOfCells) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    I18n::Message titles[k_totalNumberOfCells] = {I18n::Message::EditProgram, I18n::Message::Console};
    // TODO: translate Console in the .i18n
    myCell->setMessage(titles[index]);
  }
}

int MenuController::numberOfButtons(ButtonRowController::Position position) const {
  return 1;
}

Button * MenuController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  assert(index == 0);
  return (Button *) (&m_consoleButton);
}

}
