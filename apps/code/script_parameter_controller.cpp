#include "script_parameter_controller.h"

#include "menu_controller.h"

using namespace Escher;

namespace Code {

ScriptParameterController::ScriptParameterController(
    Responder *parentResponder, I18n::Message title,
    MenuController *menuController)
    : ExplicitSelectableListViewController(parentResponder),
      m_pageTitle(title),
      m_script(Ion::Storage::Record()),
      m_menuController(menuController) {
  m_executeScript.label()->setMessage(I18n::Message::ExecuteScript);
  m_renameScript.label()->setMessage(I18n::Message::Rename);
  m_deleteScript.label()->setMessage(I18n::Message::DeleteScript);
  m_autoImportScript.label()->setMessage(I18n::Message::AutoImportScript);
  m_autoImportScript.subLabel()->setMessage(
      I18n::Message::AutoImportScriptSubLabel);
}

void ScriptParameterController::setScript(Script script) { m_script = script; }

void ScriptParameterController::dismissScriptParameterController() {
  m_script = Script(Ion::Storage::Record());
  stackViewController()->pop();
}

const char *ScriptParameterController::title() {
  return I18n::translate(m_pageTitle);
}

bool ScriptParameterController::handleEvent(Ion::Events::Event event) {
  AbstractMenuCell *cell = static_cast<AbstractMenuCell *>(selectedCell());
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  Script s = m_script;
  if (cell == &m_executeScript) {
    dismissScriptParameterController();
    m_menuController->openConsoleWithScript(s);
  } else if (cell == &m_renameScript) {
    dismissScriptParameterController();
    m_menuController->renameSelectedScript();
  } else if (cell == &m_autoImportScript) {
    m_script.toggleAutoImportation();
    m_selectableListView.reloadSelectedCell();
    m_menuController->reloadConsole();
    Container::activeApp()->setFirstResponder(&m_selectableListView);
  } else {
    assert(cell == &m_deleteScript);
    dismissScriptParameterController();
    m_menuController->deleteScript(s);
    m_menuController->reloadConsole();
  }
  return true;
}

void ScriptParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetMemoization();
  m_selectableListView.reloadData();
  m_selectableListView.selectCell(0);
}

void ScriptParameterController::didBecomeFirstResponder() {
  selectCell(0);
  ExplicitSelectableListViewController::didBecomeFirstResponder();
}

AbstractMenuCell *ScriptParameterController::cell(int row) {
  assert(row >= 0);
  assert(row < k_totalNumberOfCell);
  AbstractMenuCell *cells[k_totalNumberOfCell] = {
      &m_executeScript, &m_renameScript, &m_autoImportScript, &m_deleteScript};
  return cells[row];
}

void ScriptParameterController::fillCellForRow(HighlightCell *cell, int row) {
  if (cell == &m_autoImportScript && !m_script.isNull()) {
    m_autoImportScript.accessory()->setState(m_script.autoImportation());
  }
}

StackViewController *ScriptParameterController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

}  // namespace Code
