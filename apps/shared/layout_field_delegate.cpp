#include "layout_field_delegate.h"

using namespace Poincare;

namespace Shared {

bool LayoutFieldDelegate::layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->layoutFieldShouldFinishEditing(layoutField, event);
}

bool LayoutFieldDelegate::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->layoutFieldDidReceiveEvent(layoutField, event);
}

bool LayoutFieldDelegate::layoutFieldDidFinishEditing(LayoutField * layoutField, Layout layoutR, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->layoutFieldDidFinishEditing(layoutField, layoutR, event);
}

bool LayoutFieldDelegate::layoutFieldDidAbortEditing(LayoutField * layoutField) {
  return expressionFieldDelegateApp()->layoutFieldDidAbortEditing(layoutField);
}

void LayoutFieldDelegate::layoutFieldDidChangeSize(LayoutField * layoutField) {
  return expressionFieldDelegateApp()->layoutFieldDidChangeSize(layoutField);
}

}
