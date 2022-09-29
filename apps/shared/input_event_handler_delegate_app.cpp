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

bool InputEventHandlerDelegateApp::handleEvent(Ion::Events::Event event) {
  /* When they want to open the store menu, handleEvent(Events::Sto) on cells
   * saves the text to be copied in the storeBuffer and return false for the
   * event to bubble up and be treated here. */
  if (event == Ion::Events::Sto) {
    AppsContainer::sharedAppsContainer()->openStoreMenu();
    Clipboard::sharedStoreBuffer()->reset();
    return true;
  }
  return App::handleEvent(event);
}

}
