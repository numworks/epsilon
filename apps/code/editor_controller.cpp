#include "editor_controller.h"
#include "menu_controller.h"
#include "script_parameter_controller.h"
#include "variable_box_controller.h"
#include <apps/code/app.h>
#include <escher/metric.h>

extern Ion::Storage storage;

namespace Code {

EditorController::EditorController(MenuController * menuController) :
  ViewController(nullptr),
  m_textArea(this),
  m_areaBuffer(nullptr),
  m_script(Ion::Storage::Record()),
  m_menuController(menuController)
{
  m_textArea.setDelegate(this);
}

EditorController::~EditorController() {
  delete m_areaBuffer;
  m_areaBuffer = nullptr;
}

void EditorController::setScript(Script script) {
  m_script = script;
  const char * scriptBody = m_script.readContent();
  size_t scriptBodySize = strlen(scriptBody)+1;
  size_t availableScriptSize = scriptBodySize + storage.availableSize();
  assert(m_areaBuffer == nullptr);
  m_areaBuffer = new char[availableScriptSize];
  strlcpy(m_areaBuffer, scriptBody, scriptBodySize);
  m_textArea.setText(m_areaBuffer, availableScriptSize);
}

// TODO: this should be done in textAreaDidFinishEditing maybe??
bool EditorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::Back) {
    Script::ErrorStatus err = m_script.writeContent(m_areaBuffer, strlen(m_areaBuffer)+1);
    if (err == Script::ErrorStatus::NotEnoughSpaceAvailable || err == Script::ErrorStatus::RecordDoesNotExist) {
      assert(false); // This should not happen as we set the text area according to the available space in the Kallax
    } else {
      stackController()->pop();
    }
    return true;
  }
  return false;
}

void EditorController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textArea);
}

void EditorController::viewWillAppear() {
  m_textArea.setCursorLocation(strlen(m_textArea.text()));
}

void EditorController::viewDidDisappear() {
  m_menuController->scriptContentEditionDidFinish();
  delete[] m_areaBuffer;
  m_areaBuffer = nullptr;
}

bool EditorController::textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) {
  if (static_cast<App *>(textArea->app())->textInputDidReceiveEvent(textArea, event)) {
    return true;
  }
  if (event == Ion::Events::EXE) {
    // Auto-Indent
    char * text = const_cast<char *>(textArea->text());
    int charBeforeCursorIndex = textArea->cursorLocation()-1;
    int indentationSize = 0;
    // Indent more if the previous line ends with ':'.
    if (charBeforeCursorIndex >= 0 && text[charBeforeCursorIndex] == ':') {
      indentationSize += k_indentationSpacesNumber;
    }
    // Compute the indentation of the current line.
    int indentationIndex = charBeforeCursorIndex;
    while (indentationIndex >= 0 && text[indentationIndex] != '\n') {
      indentationIndex--;
    }
    if (indentationIndex >= 0) {
      indentationIndex++;
      while (text[indentationIndex] == ' ') {
        indentationSize++;
        indentationIndex++;
      }
    }
    char indentationBuffer[50]; // FIXME
    indentationBuffer[0] = '\n';
    for (int i = 0; i < indentationSize; i++) {
      indentationBuffer[i+1] = ' ';
    }
    indentationBuffer[indentationSize+1] = 0;
    textArea->handleEventWithText(indentationBuffer);
    return true;
  }

  if (event == Ion::Events::Backspace) {
    // If the cursor is on the left of the text of a line,
    // backspace one intentation space at a time.
    char * text = const_cast<char *>(textArea->text());
    int charBeforeCursorIndex = textArea->cursorLocation()-1;
    int indentationSize = 0;
    while (charBeforeCursorIndex >= 0 && text[charBeforeCursorIndex] == ' ') {
      charBeforeCursorIndex--;
      indentationSize++;
    }
    if (charBeforeCursorIndex >= 0 && text[charBeforeCursorIndex] == '\n'
        && indentationSize >= k_indentationSpacesNumber)
    {
      for (int i = 0; i < k_indentationSpacesNumber; i++) {
        textArea->removeChar();
      }
      return true;
    }
  } else if (event == Ion::Events::Space) {
    // If the cursor is on the left of the text of a line,
    // a space triggers an indentation.
    char * text = const_cast<char *>(textArea->text());
    int charBeforeCursorIndex = textArea->cursorLocation()-1;
    while (charBeforeCursorIndex >= 0 && text[charBeforeCursorIndex] == ' ') {
      charBeforeCursorIndex--;
    }
    if (charBeforeCursorIndex >= 0 && text[charBeforeCursorIndex] == '\n') {
      char indentationBuffer[k_indentationSpacesNumber+1];
      for (int i = 0; i < k_indentationSpacesNumber; i++) {
        indentationBuffer[i] = ' ';
      }
      indentationBuffer[k_indentationSpacesNumber] = 0;
      textArea->handleEventWithText(indentationBuffer);
      return true;
    }
  }
  return false;
}

Toolbox * EditorController::toolboxForTextInput(TextInput * textInput) {
  Code::App * codeApp = static_cast<Code::App *>(app());
  return codeApp->pythonToolbox();
}

StackViewController * EditorController::stackController() {
  return static_cast<StackViewController *>(parentResponder());
}

}
