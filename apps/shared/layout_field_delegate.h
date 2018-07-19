#ifndef SHARED_LAYOUT_FIELD_DELEGATE_H
#define SHARED_LAYOUT_FIELD_DELEGATE_H

#include <escher/layout_field_delegate.h>
#include "expression_field_delegate_app.h"

namespace Shared {

class LayoutFieldDelegate : public ::LayoutFieldDelegate {
public:
  bool layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::LayoutRef layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(LayoutField * layoutField) override;
  Toolbox * toolboxForLayoutField(LayoutField * layoutField) override;
private:
  virtual ExpressionFieldDelegateApp * expressionFieldDelegateApp() = 0;
};

}

#endif
