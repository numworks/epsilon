#include "script_parameter_controller.h"
#include "menu_controller.h"
#include <poincare/integer.h>

namespace Code {

ScriptParameterController::ScriptParameterController(Responder * parentResponder, I18n::Message title, MenuController * menuController) :
  ViewController(parentResponder),
  m_pageTitle(title),
  m_executeScript(I18n::Message::ExecuteScript),
  m_renameScript(I18n::Message::Rename),
  m_autoImportScript(I18n::Message::AutoImportScript),
  m_deleteScript(I18n::Message::DeleteScript),
  m_duplicateScript(I18n::Message::DuplicateScript),
  m_size(I18n::Message::ScriptSize),
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
        m_script.toggleImportationStatus();
        m_selectableTableView.reloadData();
        m_menuController->reloadConsole();
        Container::activeApp()->setFirstResponder(&m_selectableTableView);
        return true;
      case 3:{
        MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)m_selectableTableView.selectedCell();
        m_sizedisplaypercent = !m_sizedisplaypercent;
        GetScriptSize(myCell);
        return true;
      }
      case 4:
        dismissScriptParameterController();
        m_menuController->duplicateScript(s);
        m_menuController->reloadConsole();
        return true;
      case 5:
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
  HighlightCell * cells[] = {&m_executeScript, &m_renameScript, &m_autoImportScript, &m_size, &m_duplicateScript, &m_deleteScript};
  return cells[index];
}

void ScriptParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_autoImportScript) {
    SwitchView * switchView = (SwitchView *)m_autoImportScript.accessoryView();
    switchView->setState(m_script.importationStatus());
  } else if (cell == &m_size) {
    MessageTableCellWithBuffer * myCell = (MessageTableCellWithBuffer *)cell;
    GetScriptSize(myCell);
    myCell->setAccessoryFont(KDFont::SmallFont);
    myCell->setAccessoryTextColor(Palette::GreyDark);
  }
}

void ScriptParameterController::GetScriptSize(MessageTableCellWithBuffer* myCell){
  if(m_sizedisplaypercent){
    char size[18];
    int sizelen = Poincare::Integer((int)m_script.value().size).serialize(size, 6);
    size[sizelen] = ' ';
    size[sizelen+1] = 'o';
    size[sizelen+2] = ' ';
    size[sizelen+3] = '/';
    size[sizelen+4] = ' ';
    int sizelen2 = Poincare::Integer((int)Ion::Storage::k_storageSize).serialize(size+sizelen+5, 6) + sizelen + 5;
    size[sizelen2] = ' ';
    size[sizelen2+1] = 'o';
    size[sizelen2+2] = '\0';
    myCell->setAccessoryText(size);
  }else{
    char size[18];
    int sizelen = Poincare::Integer((int)(((float)((int)m_script.value().size)/((int)Ion::Storage::k_storageSize)) * 100.f)).serialize(size, 3);
    size[sizelen] = ' ';
    size[sizelen+1] = '%';
    size[sizelen+2] = '\0';
    myCell->setAccessoryText(size);
  }
}

StackViewController * ScriptParameterController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

}
