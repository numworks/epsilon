#include "script_parameter_controller.h"
#include "menu_controller.h"

namespace Code {

ScriptParameterController::ScriptParameterController(Responder * parentResponder, I18n::Message title, ScriptStore * scriptStore, MenuController * menuController) :
  ViewController(parentResponder),
  m_pageTitle(title),
  m_editScript(I18n::Message::Default),
  m_renameScript(I18n::Message::Default),
  m_autoImportScript(I18n::Message::Default),
  m_deleteScript(I18n::Message::Default),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_scriptStore(scriptStore),
  m_menuController(menuController),
  m_autoImport(true),
  m_currentScriptIndex(-1)
{
}

void ScriptParameterController::setScript(int i){
  Script script = (m_scriptStore->scriptAtIndex(i, ScriptStore::EditableZone::Content));
  m_autoImport = script.autoImport();
  m_currentScriptIndex = i;
}

void ScriptParameterController::dismissScriptParameterController() {
  m_currentScriptIndex = -1;
  stackViewController()->pop();
}

View * ScriptParameterController::view() {
  return &m_selectableTableView;
}

const char * ScriptParameterController::title() {
  return I18n::translate(m_pageTitle);
}

bool ScriptParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int i = m_currentScriptIndex;
    switch (selectedRow()) {
      case 0:
        //TODO
        //stackViewController()->push(&m_editorController);
        return true;
      case 1:
        dismissScriptParameterController();
        m_menuController->renameSelectedScript();
        return true;
      case 2:
        m_scriptStore->switchAutoImportAtIndex(i);
        m_autoImport = !m_autoImport;
        m_selectableTableView.reloadData();
        m_menuController->reloadConsole();
        app()->setFirstResponder(&m_selectableTableView);
        return true;
      case 3:
        dismissScriptParameterController();
        m_menuController->deleteScriptAtIndex(i);
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
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0,0);
}

void ScriptParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

KDCoordinate ScriptParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

HighlightCell * ScriptParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_editScript, &m_renameScript, &m_autoImportScript, &m_deleteScript};
  return cells[index];
}

int ScriptParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

int ScriptParameterController::numberOfRows() {
  return k_totalNumberOfCell;
}

void ScriptParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_autoImportScript) {
    SwitchView * switchView = (SwitchView *)m_autoImportScript.accessoryView();
    switchView->setState(m_autoImport);
  }
  MessageTableCell * myCell = static_cast<MessageTableCell *>(cell);
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::EditScript, I18n::Message::RenameScript, I18n::Message::AutoImportScript, I18n::Message::DeleteScript};
  myCell->setMessage(labels[index]);
}

StackViewController * ScriptParameterController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

}
