#include "editor_controller.h"

#include <escher/metric.h>
#include <ion.h>

#include "app.h"
#include "menu_controller.h"
#include "script_parameter_controller.h"

using namespace Shared;
using namespace Escher;

namespace Code {

EditorController::EditorController(MenuController* menuController,
                                   App* pythonDelegate)
    : ViewController(nullptr),
      m_editorView(this, pythonDelegate),
      m_script(Ion::Storage::Record()),
      m_scriptIndex(-1),
      m_menuController(menuController) {}

void EditorController::setScript(Script script, int scriptIndex) {
  m_script = script;
  m_scriptIndex = scriptIndex;

  /* We edit the script directly in the storage buffer. We thus put all the
   * storage available space at the end of the current edited script and we set
   * its size.
   *
   * |****|****|m_script|****|**********|¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨|
   *                                          available space
   * is transformed to:
   *
   * |****|****|m_script|¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨|****|**********|
   *                          available space
   *
   * */

  Ion::Storage::FileSystem::sharedFileSystem->putAvailableSpaceAtEndOfRecord(
      m_script);
  m_editorView.setText(const_cast<char*>(m_script.content()),
                       m_script.contentSize());
}

void EditorController::willExitApp() { cleanStorageEmptySpace(); }

bool EditorController::handleEvent(Ion::Events::Event event) {
  // TODO: this should be done in textAreaDidFinishEditing maybe??
  if (event == Ion::Events::OK || event == Ion::Events::Back ||
      event == Ion::Events::USBEnumeration) {
    /* Exit the edition on USB enumeration, because the storage needs to be in a
     * "clean" state (with all records packed at the beginning of the storage)
     */
    assert(!m_editorView.isAutocompleting());
    cleanStorageEmptySpace();
    stackController()->pop();
    return event != Ion::Events::USBEnumeration;
  }
  return false;
}

void EditorController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(&m_editorView);
  } else {
    ViewController::handleResponderChainEvent(event);
  }
}

void EditorController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_editorView.loadSyntaxHighlighter();
  m_editorView.setCursorLocation(m_editorView.text() +
                                 strlen(m_editorView.text()));
}

void EditorController::viewDidDisappear() {
  m_editorView.resetSelection();
  m_editorView.removeAutocompletionText();
  m_menuController->scriptContentEditionDidFinish();
}

StackViewController* EditorController::stackController() {
  return static_cast<StackViewController*>(parentResponder());
}

void EditorController::cleanStorageEmptySpace() {
  if (m_script.isNull() ||
      !Ion::Storage::FileSystem::sharedFileSystem->hasRecord(m_script)) {
    return;
  }
  Ion::Storage::FileSystem::sharedFileSystem->getAvailableSpaceFromEndOfRecord(
      m_script, m_script.usedSize());
}

}  // namespace Code
