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

Toolbox * InputEventHandlerDelegateApp::toolboxForInputEventHandler(InputEventHandler * textInput) {
  Toolbox * toolbox = AppsContainer::sharedAppsContainer()->mathToolbox();
  toolbox->setSender(textInput);
  return toolbox;
}

NestedMenuController * InputEventHandlerDelegateApp::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  VariableBoxController * varBox = AppsContainer::sharedAppsContainer()->variableBoxController();
  varBox->setSender(textInput);
  varBox->lockDeleteEvent(VariableBoxController::Page::RootMenu);
  return varBox;
}

}
