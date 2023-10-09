#ifndef CALCULATION_JUNIOR_MATH_FIELD_DELEGATE_H
#define CALCULATION_JUNIOR_MATH_FIELD_DELEGATE_H

#include <apps/shared/math_field_delegate.h>
#include <poincare/store.h>

#include "layout_field.h"
#include "layout_field_delegate.h"

namespace CalculationJunior {

class MathFieldDelegate : public Shared::AbstractMathFieldDelegate,
                          public LayoutFieldDelegate {
  friend class StoreMenuController;
  friend class MathVariableBoxController;

 public:
  virtual ~MathFieldDelegate() = default;
  bool layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                  Ion::Events::Event event) override;
#if 0
  bool isAcceptableExpression(const Poincare::Expression expression) override;
  void storeValue(const char* text = "") override;
#endif

#if 0
 protected:
  bool handleEvent(Ion::Events::Event event) override;
  bool isStoreMenuOpen() const;

 private:
  StoreMenuController m_storeMenuController;
#endif
};

}  // namespace CalculationJunior

#endif
