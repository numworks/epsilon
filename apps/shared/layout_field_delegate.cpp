#include "layout_field_delegate.h"

#include "layout_field_delegate_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

bool LayoutFieldDelegate::layoutFieldShouldFinishEditing(
    LayoutField *layoutField, Ion::Events::Event event) {
  return layoutFieldDelegateApp()->layoutFieldShouldFinishEditing(layoutField,
                                                                  event);
}

bool LayoutFieldDelegate::layoutFieldDidReceiveEvent(LayoutField *layoutField,
                                                     Ion::Events::Event event) {
  return layoutFieldDelegateApp()->layoutFieldDidReceiveEvent(layoutField,
                                                              event);
}

bool LayoutFieldDelegate::layoutFieldDidFinishEditing(
    LayoutField *layoutField, Layout layoutR, Ion::Events::Event event) {
  return layoutFieldDelegateApp()->layoutFieldDidFinishEditing(layoutField,
                                                               layoutR, event);
}

void LayoutFieldDelegate::layoutFieldDidAbortEditing(LayoutField *layoutField) {
  layoutFieldDelegateApp()->layoutFieldDidAbortEditing(layoutField);
}

void LayoutFieldDelegate::layoutFieldDidChangeSize(LayoutField *layoutField) {
  return layoutFieldDelegateApp()->layoutFieldDidChangeSize(layoutField);
}

LayoutFieldDelegateApp *LayoutFieldDelegate::layoutFieldDelegateApp() const {
  return static_cast<LayoutFieldDelegateApp *>(Escher::Container::activeApp());
}

}  // namespace Shared
