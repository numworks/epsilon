#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <cmath>
#include <string.h>

using namespace Poincare;

namespace Shared {

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  ::App(container, snapshot, rootViewController, I18n::Message::Warning),
  TextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  return container()->globalContext();
}

AppsContainer * TextFieldDelegateApp::container() {
  return (AppsContainer *)app()->container();
}

const char * TextFieldDelegateApp::XNT() {
  return "X";
}

bool TextFieldDelegateApp::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return isFinishingEvent(event);
}

bool TextFieldDelegateApp::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event)) {
    if (unparsableText(textField->text(), textField)) {
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    forceEdition(textField);
    return displayVariableBoxController(textField);
  }
  if (event == Ion::Events::XNT) {
    forceEdition(textField);
    const char * xnt = privateXNT(textField);
    return textField->handleEventWithText(xnt);
  }
  return false;
}

Toolbox * TextFieldDelegateApp::toolboxForTextInput(TextInput * textInput) {
  Toolbox * toolbox = container()->mathToolbox();
  toolbox->setSender(textInput);
  return toolbox;
}

/* Protected */

void TextFieldDelegateApp::forceEdition(TextField * textField) {
  if (!textField->isEditing()) {
    textField->setEditing(true);
  }
}

bool TextFieldDelegateApp::isFinishingEvent(Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool TextFieldDelegateApp::unparsableText(const char * text, Responder * responder) {
  Expression exp = Expression::parse(text);
  if (exp.isUninitialized()) {
    responder->app()->displayWarning(I18n::Message::SyntaxError);
    return true;
  }
  return false;
}

bool TextFieldDelegateApp::displayVariableBoxController(Responder * sender) {
  AppsContainer * appsContainer = (AppsContainer *)sender->app()->container();
  VariableBoxController * variableBoxController = appsContainer->variableBoxController();
  variableBoxController->setSender(sender);
  sender->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
  return true;
}

/* Private */

const char * TextFieldDelegateApp::privateXNT(TextField * textField) {
  static constexpr struct { const char *name, *xnt; } sFunctions[] = {
    { "diff", "x" }, { "int", "x" },
    { "product", "n" }, { "sum", "n" }
  };
  // Let's assume everything before the cursor is nested correctly, which is reasonable if the expression is being entered left-to-right.
  const char * text = textField->text();
  size_t location = textField->cursorLocation();
  unsigned level = 0;
  while (location >= 1) {
    location--;
    switch (text[location]) {
      case '(':
        // Check if we are skipping to the next matching '('.
        if (level) {
          level--;
          break;
        }
        // Skip over whitespace.
        while (location >= 1 && text[location-1] == ' ') {
          location--;
        }
        // We found the next innermost function we are currently in.
        for (size_t i = 0; i < sizeof(sFunctions)/sizeof(sFunctions[0]); i++) {
          const char * name = sFunctions[i].name;
          size_t length = strlen(name);
          if (location >= length && memcmp(&text[location-length], name, length) == 0) {
            return sFunctions[i].xnt;
          }
        }
        break;
      case ',':
        // Commas encountered while skipping to the next matching '(' should be ignored.
        if (level) {
          break;
        }
        // FALLTHROUGH
      case ')':
        // Skip to the next matching '('.
        level++;
        break;
    }
  }
  // Fallback to the default
  return XNT();
}

}
