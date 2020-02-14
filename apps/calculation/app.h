#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "calculation_store.h"
#include "edit_expression_controller.h"
#include "history_controller.h"
#include "../shared/text_field_delegate_app.h"
#include <escher.h>

namespace Calculation {

class App : public Shared::ExpressionFieldDelegateApp {
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
    CalculationStore * calculationStore() { return &m_calculationStore; }
  private:
    void tidy() override;
    CalculationStore m_calculationStore;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  TELEMETRY_ID("Calculation");
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) override;
  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;
private:
  App(Snapshot * snapshot);
  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
