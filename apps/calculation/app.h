#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "edit_expression_controller.h"
#include "local_context.h"
#include "history_controller.h"
#include "../shared/text_field_delegate_app.h"
#include <escher.h>

namespace Calculation {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    App * build(Container * container) override;
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  static Descriptor * buildDescriptor();
  Poincare::Context * localContext() override;
private:
  App(Container * container, Descriptor * descriptor);
  LocalContext m_localContext;
  CalculationStore m_calculationStore;
  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
