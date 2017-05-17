#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include <poincare.h>
#include "text_field_delegate_app.h"

class AppsContainer;

namespace Shared {

class FunctionApp : public TextFieldDelegateApp {
public:
  FunctionApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
  virtual ~FunctionApp() = default;
  virtual InputViewController * inputViewController() = 0;
  void willBecomeInactive() override;
};

}

#endif
