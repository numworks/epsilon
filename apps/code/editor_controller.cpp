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
  Script::Data scriptData = m_script.value();
  size_t availableScriptSize = scriptData.size + Ion::Storage::sharedStorage()->availableSize();
  assert(sizeof(m_areaBuffer) >= availableScriptSize);
  // We cannot use strlcpy as the first char reprensenting the importation status can be 0.
  memcpy(m_areaBuffer, (const char *)scriptData.buffer, scriptData.size);
  m_editorView.setText(m_areaBuffer+1, availableScriptSize-1); // 1 char is taken by the importation status flag
}

// TODO: this should be done in textAreaDidFinishEditing maybe??
bool EditorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::Back || event == Ion::Events::Home) {
    saveScript();
    stackController()->pop();
    return event != Ion::Events::Home;
  }
  return false;
}

void EditorController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_editorView);
}

void EditorController::viewWillAppear() {
  m_editorView.loadSyntaxHighlighter();
  m_editorView.setCursorLocation(strlen(m_editorView.text()));
}

void EditorController::viewDidDisappear() {
  m_menuController->scriptContentEditionDidFinish();
}

bool EditorController::textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) {
  if (event == Ion::Events::Var) {
    // We save script before displaying the Variable box to add new functions or variables
    saveScript();
    return false;
  }
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
    textArea->handleEventWithText("\n");
    for (int i = 0; i < indentationSize; i++) {
      textArea->handleEventWithText(" ");
    }
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

VariableBoxController * EditorController::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  VariableBoxController * varBox = static_cast<App *>(app())->variableBoxController();
  varBox->loadFunctionsAndVariables();
  return varBox;
}

InputEventHandlerDelegateApp * EditorController::inputEventHandlerDelegateApp() {
  return static_cast<App *>(app());
}

StackViewController * EditorController::stackController() {
  return static_cast<StackViewController *>(parentResponder());
}

void EditorController::saveScript() {
  size_t sizeOfValue = strlen(m_areaBuffer+1)+1+1; // size of scriptContent + size of importation status
  Script::ErrorStatus err = m_script.setValue({.buffer=m_areaBuffer, .size=sizeOfValue});
  if (err == Script::ErrorStatus::NotEnoughSpaceAvailable || err == Script::ErrorStatus::RecordDoesNotExist) {
    assert(false); // This should not happen as we set the text area according to the available space in the Kallax
  }
}

}
