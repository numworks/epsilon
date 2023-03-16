#ifndef CALCULATION_JUNIOR_APP_H
#define CALCULATION_JUNIOR_APP_H

#include <apps/shared/shared_app.h>

#include "expression_field_delegate_app.h"
#include "layout_field.h"
#include "main_controller.h"

namespace CalculationJunior {

class App : public ExpressionFieldDelegateApp {
 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image* icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
  };
  bool layoutFieldDidHandleEvent(LayoutField* layoutField, bool returnValue,
                                 bool layoutDidChange) override {
    return layoutDidChange || returnValue;
  }
  bool layoutFieldShouldFinishEditing(LayoutField* layoutField,
                                      Ion::Events::Event event) override {
    return false;
  }

 private:
  App(Snapshot* snapshot);
  MainController m_mainController;
  Escher::StackViewController m_stackViewController;
};

}  // namespace CalculationJunior

#endif
