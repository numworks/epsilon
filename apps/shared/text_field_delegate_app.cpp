#include "text_field_delegate_app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, ViewController * rootViewController, const char * name,
  const char * upperName, const Image * icon) :
  ::App(container, rootViewController, name, upperName, icon),
  ExpressionTextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  return container()->globalContext();
}

AppsContainer * TextFieldDelegateApp::container() {
  return (AppsContainer *)app()->container();
}

}
