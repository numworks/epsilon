#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "calculation_store.h"
#include "edit_expression_controller.h"
#include "history_controller.h"
#include "local_context.h"
#include "../shared/text_field_delegate_app.h"
#include <escher.h>

namespace Calculation {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    CalculationStore * calculationStore();
  private:
    void tidy() override;
    CalculationStore m_calculationStore;
  };
  Poincare::Context * localContext() override;
private:
  App(Container * container, Snapshot * snapshot);
  LocalContext m_localContext;
  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
