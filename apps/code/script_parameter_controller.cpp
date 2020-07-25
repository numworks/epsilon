#include "script_parameter_controller.h"
#include "menu_controller.h"

namespace Code {

ScriptParameterController::ScriptParameterController(Responder * parentResponder, I18n::Message title, MenuController * menuController) :
  ViewController(parentResponder),
  m_pageTitle(title),
  m_executeScript(I18n::Message::ExecuteScript),
  m_renameScript(I18n::Message::Rename),
  m_autoImportScript(I18n::Message::AutoImportScript),
  m_deleteScript(I18n::Message::DeleteScript),
  m_selectableTableView(this),
  m_script(Ion::Storage::Record()),
  m_menuController(menuController)
{
}

void ScriptParameterController::setScript(Script script){
  m_script = script;
}

void ScriptParameterController::dismissScriptParameterController() {
  m_script = Script(Ion::Storage::Record());
  stackViewController()->pop();
}

const char * ScriptParameterController::title() {
  return I18n::translate(m_pageTitle);
}

bool ScriptParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Script s = m_script;
    switch (selectedRow()) {
      case 0:
        dismissScriptParameterController();
        m_menuController->openConsoleWithScript(s);
        return true;
      case 1:
        dismissScriptParameterController();
        m_menuController->renameSelectedScript();
        return true;
      case 2:
        m_script.toggleAutoimportationStatus();
        m_selectableTableView.reloadData();
        m_menuController->reloadConsole();
        Container::activeApp()->setFirstResponder(&m_selectableTableView);
        return true;
      case 3:
        dismissScriptParameterController();
        m_menuController->deleteScript(s);
        m_menuController->reloadConsole();
        return true;
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

void ScriptParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0,0);
}

void ScriptParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

HighlightCell * ScriptParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_executeScript, &m_renameScript, &m_autoImportScript, &m_deleteScript};
  return cells[index];
}

void ScriptParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_autoImportScript) {
    SwitchView * switchView = (SwitchView *)m_autoImportScript.accessoryView();
    switchView->setState(m_script.autoImportationStatus());
  }
}

StackViewController * ScriptParameterController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

}
