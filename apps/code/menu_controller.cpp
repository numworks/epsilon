#include "menu_controller.h"
#include "../i18n.h"
#include <assert.h>

namespace Code {

MenuController::MenuController(Responder * parentResponder, ScriptStore * scriptStore, ButtonRowController * footer) :
  ViewController(parentResponder),
  ButtonRowDelegate(nullptr, footer),
  m_scriptStore(scriptStore),
  m_addNewScriptCell(I18n::Message::AddScript),
  m_consoleButton(this, I18n::Message::Console, Invocation([](void * context, void * sender) {
    MenuController * menu = (MenuController *)context;
    menu->app()->displayModalViewController(menu->consoleController(), 0.5f, 0.5f);
  }, this)),
  m_selectableTableView(this, this, 0, 1, 0, 0, 0, 0, this, nullptr, false),
  m_consoleController(parentResponder, m_scriptStore),
  m_scriptParameterController(nullptr, I18n::Message::ScriptOptions, m_scriptStore, this)
{
  for (int i = 0; i< k_maxNumberOfCells; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
  }
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
    if (m_selectableTableView.selectedRow()<0) {
      footer()->setSelectedButton(-1);
      m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
      app()->setFirstResponder(&m_selectableTableView);
      return true;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int selectedRow = m_selectableTableView.selectedRow();
    if (selectedRow >= 0 && selectedRow < m_scriptStore->numberOfScripts()) {
      configureScript();
      return true;
    } else if (selectedRow == m_scriptStore->numberOfScripts()) {
      addScript();
      return true;
    }
  }
  return false;
}

void MenuController::configureScript() {
  m_scriptParameterController.setScript(m_selectableTableView.selectedRow());
  stackViewController()->push(&m_scriptParameterController);
}

void MenuController::addScript() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  m_scriptStore->addNewScript();
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, numberOfRows()-2);
}

void MenuController::deleteScriptAtIndex(int i) {
  m_scriptStore->deleteScript(i);
  m_selectableTableView.reloadData();
}

int MenuController::numberOfRows() {
  return m_scriptStore->numberOfScripts() + 1;
  //TODO do not add the addScript row if there can be no more scripts stored.
};

KDCoordinate MenuController::cellHeight() {
  return k_rowHeight;
}

HighlightCell * MenuController::reusableCell(int index) {
  assert(index >= 0);
  if (index < m_scriptStore->numberOfScripts()) {
    return &m_cells[index];
  }
  assert(index == m_scriptStore->numberOfScripts());
  return &m_addNewScriptCell;
}

int MenuController::reusableCellCount() {
  return m_scriptStore->numberOfScripts() + 1;
}

void MenuController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index < m_scriptStore->numberOfScripts()) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    // TODO: store script names
    myCell->setMessage(I18n::Message::Console);
  }
}

int MenuController::numberOfButtons(ButtonRowController::Position position) const {
  return 1;
}

Button * MenuController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  assert(index == 0);
  return const_cast<Button *>(&m_consoleButton);
}

StackViewController * MenuController::stackViewController() {
 return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

}
