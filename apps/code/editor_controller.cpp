#include "editor_controller.h"
#include "menu_controller.h"
#include "script_parameter_controller.h"
#include "app.h"
#include <escher/metric.h>
#include <ion.h>

using namespace Shared;

namespace Code {

EditorController::EditorController(MenuController * menuController, App * pythonDelegate) :
  ViewController(nullptr),
  m_editorView(this, pythonDelegate),
  m_script(Ion::Storage::Record()),
  m_menuController(menuController)
{
  m_editorView.setTextAreaDelegates(this, this);
}

void EditorController::setScript(Script script) {
  m_script = script;

  /* We edit the script direclty in the storage buffer. We thus put all the
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

  size_t newScriptSize = Ion::Storage::sharedStorage()->putAvailableSpaceAtEndOfRecord(m_script);
  m_editorView.setText(const_cast<char *>(m_script.scriptContent()), newScriptSize - Script::k_importationStatusSize);
}

void EditorController::willExitApp() {
  cleanStorageEmptySpace();
}

// TODO: this should be done in textAreaDidFinishEditing maybe??
bool EditorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::Back || event == Ion::Events::Home || event == Ion::Events::USBEnumeration) {
    /* Exit the edition on USB enumeration, because the storage needs to be in a
     * "clean" state (with all records packed at the beginning of the storage) */
    cleanStorageEmptySpace();
    stackController()->pop();
    return event != Ion::Events::Home && event != Ion::Events::USBEnumeration;
  }
  return false;
}

void EditorController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_editorView);
}

void EditorController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_editorView.loadSyntaxHighlighter();
  m_editorView.setCursorLocation(m_editorView.text() + strlen(m_editorView.text()));
}

void EditorController::viewDidDisappear() {
  m_editorView.resetSelection();
  m_menuController->scriptContentEditionDidFinish();
}

bool EditorController::textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) {
  if (App::app()->textInputDidReceiveEvent(textArea, event)) {
    return true;
  }
  if (event == Ion::Events::EXE) {
    textArea->handleEventWithText("\n", true, false);
    return true;
  }

  if (event == Ion::Events::Backspace && textArea->selectionIsEmpty()) {
    /* If the cursor is on the left of the text of a line, backspace one
     * indentation space at a time. */
    const char * text = textArea->text();
    const char * cursorLocation = textArea->cursorLocation();
    const char * firstNonSpace = UTF8Helper::NotCodePointSearch(text, ' ', true, cursorLocation);
    assert(firstNonSpace >= text);
    bool cursorIsPrecededOnTheLineBySpacesOnly = false;
    size_t numberOfSpaces = cursorLocation - firstNonSpace;
    if (UTF8Helper::CodePointIs(firstNonSpace, '\n')) {
      cursorIsPrecededOnTheLineBySpacesOnly = true;
      numberOfSpaces -= UTF8Decoder::CharSizeOfCodePoint('\n');
    } else if (firstNonSpace == text) {
      cursorIsPrecededOnTheLineBySpacesOnly = true;
    }
    numberOfSpaces = numberOfSpaces / UTF8Decoder::CharSizeOfCodePoint(' ');
    if (cursorIsPrecededOnTheLineBySpacesOnly && numberOfSpaces >= TextArea::k_indentationSpaces) {
      for (int i = 0; i < TextArea::k_indentationSpaces; i++) {
        textArea->removePreviousGlyph();
      }
      return true;
    }
  } else if (event == Ion::Events::Space) {
    /* If the cursor is on the left of the text of a line, a space triggers an
     * indentation. */
    const char * text = textArea->text();
    const char * firstNonSpace = UTF8Helper::NotCodePointSearch(text, ' ', true, textArea->cursorLocation());
    assert(firstNonSpace >= text);
    if (UTF8Helper::CodePointIs(firstNonSpace, '\n')) {
      assert(UTF8Decoder::CharSizeOfCodePoint(' ') == 1);
      char indentationBuffer[TextArea::k_indentationSpaces+1];
      for (int i = 0; i < TextArea::k_indentationSpaces; i++) {
        indentationBuffer[i] = ' ';
      }
      indentationBuffer[TextArea::k_indentationSpaces] = 0;
      textArea->handleEventWithText(indentationBuffer);
      return true;
    }
  }
  return false;
}

VariableBoxController * EditorController::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  VariableBoxController * varBox = App::app()->variableBoxController();
  varBox->loadFunctionsAndVariables();
  return varBox;
}

StackViewController * EditorController::stackController() {
  return static_cast<StackViewController *>(parentResponder());
}

void EditorController::cleanStorageEmptySpace() {
  if (m_script.isNull() || !Ion::Storage::sharedStorage()->hasRecord(m_script)) {
    return;
  }
  Ion::Storage::Record::Data scriptValue = m_script.value();
  Ion::Storage::sharedStorage()->getAvailableSpaceFromEndOfRecord(
      m_script,
      scriptValue.size - Script::k_importationStatusSize - (strlen(m_script.scriptContent()) + 1)); // TODO optimize number of script fetches
}


}
