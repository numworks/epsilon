#include "layout_field_delegate.h"

#include "expression_field_delegate_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

bool LayoutFieldDelegate::layoutFieldShouldFinishEditing(
    LayoutField *layoutField, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->layoutFieldShouldFinishEditing(
      layoutField, event);
}

bool LayoutFieldDelegate::layoutFieldDidReceiveEvent(LayoutField *layoutField,
                                                     Ion::Events::Event event) {
  return expressionFieldDelegateApp()->layoutFieldDidReceiveEvent(layoutField,
                                                                  event);
}

bool LayoutFieldDelegate::layoutFieldDidFinishEditing(
    LayoutField *layoutField, Layout layoutR, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->layoutFieldDidFinishEditing(
      layoutField, layoutR, event);
}

bool LayoutFieldDelegate::layoutFieldDidAbortEditing(LayoutField *layoutField) {
  return expressionFieldDelegateApp()->layoutFieldDidAbortEditing(layoutField);
}

void LayoutFieldDelegate::layoutFieldDidChangeSize(LayoutField *layoutField) {
  return expressionFieldDelegateApp()->layoutFieldDidChangeSize(layoutField);
}

ExpressionFieldDelegateApp *LayoutFieldDelegate::expressionFieldDelegateApp()
    const {
  return static_cast<ExpressionFieldDelegateApp *>(
      Escher::Container::activeApp());
}

}  // namespace Shared
