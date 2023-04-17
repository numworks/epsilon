#ifndef SHARED_LAYOUT_FIELD_DELEGATE_APP_H
#define SHARED_LAYOUT_FIELD_DELEGATE_APP_H

#include <escher/layout_field_delegate.h>
#include <poincare/store.h>

#include "text_field_delegate_app.h"

/* TODO: This class should be refactored/deleted as well as
 * InputEventHandlerDelegateApp and TextFieldDelegateApp. */

namespace Shared {

class LayoutFieldDelegateApp : public TextFieldDelegateApp,
                               public Escher::LayoutFieldDelegate {
  friend class StoreMenuController;
  friend class MathVariableBoxController;

 public:
  virtual ~LayoutFieldDelegateApp() = default;
  bool layoutFieldShouldFinishEditing(Escher::LayoutField* layoutField,
                                      Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  bool isAcceptableExpression(Escher::EditableField* field,
                              const Poincare::Expression expression) override;
  void storeValue(const char* text = "") override;

 protected:
  LayoutFieldDelegateApp(Snapshot* snapshot,
                         Escher::ViewController* rootViewController);
  bool handleEvent(Ion::Events::Event event) override;
  bool isStoreMenuOpen() const;

 private:
  StoreMenuController m_storeMenuController;
};

}  // namespace Shared

#endif
