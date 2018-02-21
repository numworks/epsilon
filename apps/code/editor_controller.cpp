#include "editor_controller.h"
#include "menu_controller.h"
#include "script_parameter_controller.h"
#include "variable_box_controller.h"
#include "helpers.h"
#include <apps/code/app.h>
#include <escher/metric.h>

namespace Code {

EditorController::EditorController(MenuController * menuController) :
  ViewController(nullptr),
  m_textArea(this),
  m_menuController(menuController)
{
  m_textArea.setDelegate(this);
}

void EditorController::setScript(Script script){
  m_textArea.setText(script.editableContent(), script.contentBufferSize());
}

bool EditorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::Back) {
    stackController()->pop();
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
}

bool EditorController::textAreaShouldFinishEditing(TextArea * textArea, Ion::Events::Event event) {
  return false;
}

bool EditorController::textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) {
  const char * pythonText = Helpers::PythonTextForEvent(event);
  if (pythonText != nullptr) {
    textArea->handleEventWithText(pythonText);
    return true;
  }

  if (event == Ion::Events::Var) {
    VariableBoxController * varBoxController = (static_cast<App *>(textArea->app()))->scriptsVariableBoxController();
    varBoxController->setTextAreaCaller(textArea);
    textArea->app()->displayModalViewController(varBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
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
    char indentationBuffer[indentationSize+2]; // FIXME
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
  codeApp->pythonToolbox()->setAction(codeApp->toolboxActionForTextArea());
  return codeApp->pythonToolbox();
}

StackViewController * EditorController::stackController() {
  return static_cast<StackViewController *>(parentResponder());
}

}
