#ifndef SOLVER_APP_H
#define SOLVER_APP_H

#include <apps/i18n.h>
#include <apps/shared/math_app.h>

#include "equation_store.h"
#include "interval_controller.h"
#include "list_controller.h"
#include "solutions_controller.h"
#include "solver_context.h"
#include "system_of_equations.h"

namespace Solver {

class App : public Shared::MathApp {
 public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override { return I18n::Message::SolverApp; };
    I18n::Message upperName() const override {
      return I18n::Message::SolverAppCapital;
    };
    const Escher::Image* icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App* unpack(Escher::Container* container) override {
      return new (container->currentAppBuffer()) App(this);
    };
    const Descriptor* descriptor() const override;
    void reset() override;
  };

  static App* app() { return static_cast<App*>(Escher::App::app()); }
  Poincare::Context* localContext() override { return &m_context; }
  Snapshot* snapshot() const {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }

  EquationStore* equationStore() { return &m_equationStore; }
  SystemOfEquations* system() { return &m_system; }

  void openSolutionsController(bool approximateSolve,
                               bool forceIntervalSelection);
  void openIntervalController();

  void storageDidChangeForRecord(Ion::Storage::Record record) override;
  void prepareForIntrusiveStorageChange() override;

 private:
  App(Snapshot* snapshot);

  EquationStore m_equationStore;
  // Controllers
  SolutionsController m_solutionsController;
  Escher::ButtonRowController m_solutionHeader;
  IntervalController m_intervalController;
  ListController m_listController;
  Escher::ButtonRowController m_listFooter;
  Escher::StackViewController::Default m_stackViewController;
  SystemOfEquations m_system;
  SolverContext m_context;
};

}  // namespace Solver

#endif
