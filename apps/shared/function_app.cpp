#include "function_app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

FunctionApp::FunctionApp(Container * container, ViewController * rootViewController, I18n::Message name, I18n::Message upperName, const Image * icon) :
  TextFieldDelegateApp(container, rootViewController, name, upperName, icon)
{
}

void FunctionApp::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  if (inputViewController()->isDisplayingModal()) {
    inputViewController()->abortTextFieldEditionAndDismiss();
  }
  ::App::willBecomeInactive();
}

}
