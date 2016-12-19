#include "text_field_delegate_app.h"
#include "apps_container.h"

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, ViewController * rootViewController, const char * name
  , const Image * icon) :
  ::App(container, rootViewController, name, icon),
  ExpressionTextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  AppsContainer * appsContainer = (AppsContainer *)app()->container();
  return appsContainer->globalContext();
}

