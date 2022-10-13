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

Toolbox * InputEventHandlerDelegateApp::toolboxForInputEventHandler() {
  return AppsContainer::sharedAppsContainer()->mathToolbox();
}

NestedMenuController * InputEventHandlerDelegateApp::variableBoxForInputEventHandler() {
  MathVariableBoxController * varBox = AppsContainer::sharedAppsContainer()->variableBoxController();
  varBox->lockDeleteEvent(MathVariableBoxController::Page::RootMenu);
  return varBox;
}

bool InputEventHandlerDelegateApp::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Sto) {
    storeValue("");
  }
  return App::handleEvent(event);
}

void InputEventHandlerDelegateApp::storeValue(const char * text) {
  if (m_modalViewController.isDisplayingModal()) {
    return;
  }
  m_storeController.setText(text);
  displayModalViewController(&m_storeController, 0.5f, 0.f, 0, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
}

bool InputEventHandlerDelegateApp::isStoreMenuOpen() {
  return m_modalViewController.currentModalViewController() == &m_storeController;
}

}
