#include "input_event_handler_delegate_app.h"
#include "../apps_container.h"
#include <cmath>
#include <string.h>

using namespace Poincare;

namespace Shared {

InputEventHandlerDelegateApp::InputEventHandlerDelegateApp(Snapshot * snapshot, ViewController * rootViewController) :
  ::App(snapshot, rootViewController, I18n::Message::Warning),
  InputEventHandlerDelegate()
{
}

Toolbox * InputEventHandlerDelegateApp::toolboxForInputEventHandler(InputEventHandler * textInput) {
  Toolbox * toolbox = AppsContainer::sharedAppsContainer()->mathToolbox();
  toolbox->setSender(textInput);
  return toolbox;
}

NestedMenuController * InputEventHandlerDelegateApp::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  MathVariableBoxController * varBox = AppsContainer::sharedAppsContainer()->variableBoxController();
  varBox->setSender(textInput);
  varBox->lockDeleteEvent(MathVariableBoxController::Page::RootMenu);
  return varBox;
}

}
