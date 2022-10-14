#include "input_event_handler_delegate_app.h"
#include "../apps_container.h"
#include <cmath>
#include <escher/clipboard.h>
#include <string.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

InputEventHandlerDelegateApp::InputEventHandlerDelegateApp(Snapshot * snapshot, ViewController * rootViewController) :
  ::App(snapshot, rootViewController, I18n::Message::Warning),
  InputEventHandlerDelegate()
{
}

PervasiveBox * InputEventHandlerDelegateApp::toolbox() {
  return AppsContainer::sharedAppsContainer()->mathToolbox();
}

PervasiveBox * InputEventHandlerDelegateApp::variableBox() {
  MathVariableBoxController * varBox = AppsContainer::sharedAppsContainer()->variableBoxController();
  varBox->lockDeleteEvent(MathVariableBoxController::Page::RootMenu);
  return varBox;
}

}
