#include "text_field_delegate_app.h"
#include "../apps_container.h"

using namespace Poincare;

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, ViewController * rootViewController, const char * name,
  const char * upperName, const Image * icon) :
  ::App(container, rootViewController, name, upperName, icon),
  ExpressionTextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  return appsContainer->globalContext();
}

