#ifndef CALCULATION_JUNIOR_LAYOUT_FIELD_DELEGATE_H
#define CALCULATION_JUNIOR_LAYOUT_FIELD_DELEGATE_H

#include <escher/context_provider.h>
#include <ion/events.h>
#include <poincare_junior/include/layout.h>

namespace CalculationJunior {

class LayoutField;

class LayoutFieldDelegate : public Escher::ContextProvider {
 public:
  virtual bool layoutFieldShouldFinishEditing(LayoutField* layoutField,
                                              Ion::Events::Event event) = 0;
  virtual bool layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                          Ion::Events::Event event) = 0;
  virtual bool layoutFieldDidFinishEditing(LayoutField* layoutField,
                                           PoincareJ::Layout layoutR,
                                           Ion::Events::Event event) {
    return false;
  }
  virtual bool layoutFieldDidAbortEditing(LayoutField* layoutField) {
    return false;
  }
  virtual bool layoutFieldDidHandleEvent(LayoutField* layoutField,
                                         bool returnValue,
                                         bool layoutDidChange) {
    return returnValue;
  }
  virtual void layoutFieldDidChangeSize(LayoutField* layoutField) {}
};

}  // namespace CalculationJunior

#endif
