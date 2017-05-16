#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include <poincare.h>
#include "text_field_delegate_app.h"

class AppsContainer;

namespace Shared {

class FunctionApp : public TextFieldDelegateApp {
public:
  FunctionApp(Container * container, ViewController * rootViewController, Descriptor * descriptor);
  virtual InputViewController * inputViewController() = 0;
  void willBecomeInactive() override;
};

}

#endif
