#include "function_app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

FunctionApp::FunctionApp(Container * container, ViewController * rootViewController, Descriptor * descriptor) :
  TextFieldDelegateApp(container, rootViewController, descriptor)
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
