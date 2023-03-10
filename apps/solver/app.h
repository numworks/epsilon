#ifndef SOLVER_APP_H
#define SOLVER_APP_H

#include <apps/i18n.h>

#include "../shared/expression_field_delegate_app.h"
#include "../shared/shared_app.h"
#include "equation_store.h"
#include "interval_controller.h"
#include "list_controller.h"
#include "solutions_controller.h"
#include "solver_context.h"
#include "system_of_equations.h"

namespace Solver {

class App : public Shared::ExpressionFieldDelegateApp {
 public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override { return I18n::Message::SolverApp; };
    I18n::Message upperName() const override {
      return I18n::Message::SolverAppCapital;
    };
    const Escher::Image *icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App *unpack(Escher::Container *container) override {
      return new (container->currentAppBuffer()) App(this);
    };
    const Descriptor *descriptor() const override;
    void reset() override;
    EquationStore *equationStore() { return &m_equationStore; }

   private:
    void tidy() override;
    EquationStore m_equationStore;
  };

  static App *app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Poincare::Context *localContext() override { return &m_context; }
  Snapshot *snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }

  EquationStore *equationStore() { return snapshot()->equationStore(); }
  SystemOfEquations *system() { return &m_system; }
  Escher::ViewController *solutionsControllerStack() {
    return &m_alternateEmptyViewController;
  }
  Escher::ViewController *intervalController() { return &m_intervalController; }
  SolutionsController *solutionsController() { return &m_solutionsController; }

  void storageDidChangeForRecord(Ion::Storage::Record record) override;
  void prepareForIntrusiveStorageChange() override;

  TELEMETRY_ID("Solver");

 private:
  App(Snapshot *snapshot);
  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;

  // Controllers
  SolutionsController m_solutionsController;
  IntervalController m_intervalController;
  Escher::AlternateEmptyViewController m_alternateEmptyViewController;
  ListController m_listController;
  Escher::ButtonRowController m_listFooter;
  Escher::StackViewController m_stackViewController;
  SystemOfEquations m_system;
  SolverContext m_context;
};

}  // namespace Solver

#endif
