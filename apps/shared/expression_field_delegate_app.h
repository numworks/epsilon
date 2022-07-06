#ifndef SHARED_EXPRESSION_FIELD_DELEGATE_APP_H
#define SHARED_EXPRESSION_FIELD_DELEGATE_APP_H

#include "text_field_delegate_app.h"
#include <escher/layout_field_delegate.h>

namespace Shared {

class ExpressionFieldDelegateApp : public TextFieldDelegateApp, public LayoutFieldDelegate {
public:
  virtual ~ExpressionFieldDelegateApp() = default;
  void layoutFieldDidReceiveNoneXNTEvent() override { AppsContainer::sharedAppsContainer()->resetXNT(); }
  bool layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) override;
protected:
  ExpressionFieldDelegateApp(Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
