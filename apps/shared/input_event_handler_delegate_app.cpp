#include "input_event_handler_delegate_app.h"
#include "../apps_container.h"
#include <cmath>
#include <string.h>

using namespace Poincare;

namespace Shared {

InputEventHandlerDelegateApp::InputEventHandlerDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  ::App(container, snapshot, rootViewController, I18n::Message::Warning),
  InputEventHandlerDelegate()
{
}

AppsContainer * InputEventHandlerDelegateApp::container() {
  return static_cast<AppsContainer *>(const_cast<Container *>(::App::container()));
}

Toolbox * InputEventHandlerDelegateApp::toolboxForInputEventHandler(InputEventHandler * textInput) {
  Toolbox * toolbox = container()->mathToolbox();
  toolbox->setSender(textInput);
  return toolbox;
}

NestedMenuController * InputEventHandlerDelegateApp::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  VariableBoxController * varBox = container()->variableBoxController();
  varBox->setSender(textInput);
  varBox->lockDeleteEvent(VariableBoxController::Page::RootMenu);
  return varBox;
}

}
