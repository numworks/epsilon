#ifndef SHARED_LAYOUT_FIELD_DELEGATE_H
#define SHARED_LAYOUT_FIELD_DELEGATE_H

#include "expression_field_delegate_app.h"

namespace Shared {

class LayoutFieldDelegate : public ::LayoutFieldDelegate {
public:
  bool layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layoutR, Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(LayoutField * layoutField) override;
  void layoutFieldDidChangeSize(LayoutField * layoutField) override;
  Poincare::Context * context() const override { return expressionFieldDelegateApp()->localContext(); }
protected:
  ExpressionFieldDelegateApp * expressionFieldDelegateApp() const {
    return static_cast<ExpressionFieldDelegateApp *>(Container::activeApp());
  }
};

}

#endif
