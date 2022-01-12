#ifndef SOLVER_SOLVER_APP_H
#define SOLVER_SOLVER_APP_H

#include "../shared/expression_field_delegate_app.h"
#include "list_controller.h"
#include "equation_store.h"
#include "interval_controller.h"
#include "solutions_controller.h"
#include "../shared/shared_app.h"

namespace Solver {

class App : public Shared::ExpressionFieldDelegateApp {
public:
  class Descriptor : public Escher::App::Descriptor {
    public:
      I18n::Message name() const override;
      I18n::Message upperName() const override;
      const Escher::Image * icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
    void reset() override;
    EquationStore * equationStore() { return &m_equationStore; }
    void storageDidChangeForRecord(const Ion::Storage::Record record) override;
  private:
    void tidy() override;
    EquationStore m_equationStore;
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot * snapshot() {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }
  EquationStore * equationStore() {
    return snapshot()->equationStore();
  }
  Escher::InputViewController * inputViewController() { return &m_inputViewController; }
  Escher::ViewController * solutionsControllerStack() { return &m_alternateEmptyViewController; }
  Escher::ViewController * intervalController() { return &m_intervalController; }
  SolutionsController * solutionsController() { return &m_solutionsController; }
  TELEMETRY_ID("Solver");
private:
  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;

  App(Snapshot * snapshot);
  SolutionsController m_solutionsController;
  IntervalController m_intervalController;
  Escher::AlternateEmptyViewController m_alternateEmptyViewController;
  ListController m_listController;
  Escher::ButtonRowController m_listFooter;
  Escher::StackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
};

}

#endif
