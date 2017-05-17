#include "function_app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

FunctionApp::FunctionApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController)
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
