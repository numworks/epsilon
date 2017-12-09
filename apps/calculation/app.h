#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "calculation_store.h"
#include "calculation_icon.h"
#include "edit_expression_controller.h"
#include "history_controller.h"
#include "../shared/text_field_delegate_app.h"
#include <escher.h>

namespace Calculation {

class App final : public Shared::ExpressionFieldDelegateApp {
public:
  class Descriptor final : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::CalculApp;
    }
    I18n::Message upperName() override {
      return I18n::Message::CalculAppCapital;
    }
    const Image * icon() override {
      return ImageStore::CalculationIcon;
    }
  };
  class Snapshot final : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override {
      return new App(container, this);
    }
    void reset() override {
      m_calculationStore.deleteAll();
    }
    Descriptor * descriptor() override {
      return &s_descriptor;
    }
    CalculationStore * calculationStore() {
      return &m_calculationStore;
    }
  private:
    void tidy() override {
      m_calculationStore.tidy();
    }
    CalculationStore m_calculationStore;
    static Descriptor s_descriptor;
  };
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool textInputIsCorrect(const char * text);
  bool expressionLayoutFieldDidReceiveEvent(::ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  const char * XNT() override { return "x"; }
private:
  App(Container * container, Snapshot * snapshot);
  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
