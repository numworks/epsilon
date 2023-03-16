#ifndef CALCULATION_JUNIOR_EXPRESSION_FIELD_DELEGATE_APP_H
#define CALCULATION_JUNIOR_EXPRESSION_FIELD_DELEGATE_APP_H

#include <apps/shared/text_field_delegate_app.h>
#include <poincare/store.h>

#include "layout_field.h"
#include "layout_field_delegate.h"

namespace CalculationJunior {

class ExpressionFieldDelegateApp : public Shared::TextFieldDelegateApp,
                                   public LayoutFieldDelegate {
  friend class StoreMenuController;
  friend class MathVariableBoxController;

 public:
  virtual ~ExpressionFieldDelegateApp() = default;
  bool layoutFieldShouldFinishEditing(LayoutField* layoutField,
                                      Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                  Ion::Events::Event event) override;
#if 0
  bool isAcceptableExpression(const Poincare::Expression expression) override;
  void storeValue(const char* text = "") override;
#endif

 protected:
  ExpressionFieldDelegateApp(Snapshot* snapshot,
                             Escher::ViewController* rootViewController);
#if 0
  bool handleEvent(Ion::Events::Event event) override;
  bool isStoreMenuOpen() const;

 private:
  StoreMenuController m_storeMenuController;
#endif
};

}  // namespace CalculationJunior

#endif
