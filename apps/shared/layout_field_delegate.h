#ifndef SHARED_LAYOUT_FIELD_DELEGATE_H
#define SHARED_LAYOUT_FIELD_DELEGATE_H

#include <escher/container.h>
#include <escher/layout_field_delegate.h>

namespace Shared {

class ExpressionFieldDelegateApp;

class LayoutFieldDelegate : public Escher::LayoutFieldDelegate {
 public:
  bool layoutFieldShouldFinishEditing(Escher::LayoutField* layoutField,
                                      Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Poincare::Layout layoutR,
                                   Ion::Events::Event event) override;
  bool layoutFieldDidAbortEditing(Escher::LayoutField* layoutField) override;
  void layoutFieldDidChangeSize(Escher::LayoutField* layoutField) override;

 protected:
  ExpressionFieldDelegateApp* expressionFieldDelegateApp() const;
};

}  // namespace Shared

#endif
