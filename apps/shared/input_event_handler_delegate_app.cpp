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

NestedMenuController * InputEventHandlerDelegateApp::toolbox() {
  return AppsContainer::sharedAppsContainer()->mathToolbox();
}

NestedMenuController * InputEventHandlerDelegateApp::variableBox() {
  return AppsContainer::sharedAppsContainer()->variableBoxController();
}

bool InputEventHandlerDelegateApp::isVarBoxMenuOpen() {
  return m_modalViewController.currentModalViewController() == static_cast<ViewController *>(variableBox());
}

}
