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
  m_currentScriptIndex(-1)
{
}

void ScriptParameterController::setScript(int i){
  m_editorController.setScript(m_scriptStore->scriptAtIndex(i, ScriptStore::EditableZone::Content));
  m_currentScriptIndex = i;
}

void ScriptParameterController::dismissScriptParameterController() {
  m_currentScriptIndex = -1;
  stackController()->pop();
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
        app()->displayModalViewController(&m_editorController, 0.5f, 0.5f);
        return true;
      case 1:
        dismissScriptParameterController();
        m_menuController->renameScriptAtIndex(i);
        return true;
      case 2:
      //Auto-import TODO
        return true;
      case 3:
        dismissScriptParameterController();
        m_menuController->deleteScriptAtIndex(i);
        return true;
      default:
        assert(false);
        return false;
    }
  }
  return false;
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
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::EditScript, I18n::Message::RenameScript, I18n::Message::AutoImportScript, I18n::Message::DeleteScript};
  myCell->setMessage(labels[index]);
}

StackViewController * ScriptParameterController::stackController() {
    return static_cast<StackViewController *>(parentResponder());
}

}
