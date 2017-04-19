#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include <poincare.h>
#include "text_field_delegate_app.h"

class AppsContainer;

namespace Shared {

class FunctionApp : public TextFieldDelegateApp {
public:
  FunctionApp(Container * container, ViewController * rootViewController, I18n::Message name = (I18n::Message)0, I18n::Message upperName = (I18n::Message)0, const Image * icon = nullptr);
  virtual InputViewController * inputViewController() = 0;
  void willBecomeInactive() override;
};

}

#endif
