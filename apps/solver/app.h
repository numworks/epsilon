#ifndef SOLVER_APP_H
#define SOLVER_APP_H

#include <apps/i18n.h>
#include "../shared/expression_field_delegate_app.h"
#include "../shared/menu_controller.h"
#include "../shared/shared_app.h"
#include "equations/equation_store.h"
#include "finance/data.h"
#include "equations/list_controller.h"
#include "equations/interval_controller.h"
#include "equations/solutions_controller.h"
#include "finance/finance_menu_controller.h"
#include "finance/interest_controller.h"
#include "finance/interest_menu_controller.h"
#include "finance/finance_result_controller.h"

namespace Solver {

class App : public Shared::ExpressionFieldDelegateApp, public Shared::MenuControllerDelegate {
public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
    public:
      I18n::Message name() const override { return I18n::Message::SolverApp; };
      I18n::Message upperName() const override { return I18n::Message::SolverAppCapital; };
      const Escher::Image * icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    // Subapp
    enum class SubApp {
      Unknown = -1,
      Equation = 0,
      Finance = 1,
      NumberOfSubApps
    };
    Snapshot() : m_activeSubapp(SubApp::Unknown) {};
    App * unpack(Escher::Container * container) override {
      return new (container->currentAppBuffer()) App(this);
    };
    const Descriptor * descriptor() const override;
    void reset() override;
    EquationStore * equationStore() { return &m_equationStore; }
    void storageDidChangeForRecord(const Ion::Storage::Record record) override;
    void selectSubApp(int subAppIndex) { m_activeSubapp = static_cast<SubApp>(subAppIndex); }
    int selectedSubApp() const { return static_cast<int>(m_activeSubapp); }
  private:
    void tidy() override;
    SubApp m_activeSubapp;
    EquationStore m_equationStore;
  };

  // Shared::MenuControllerDelegate
  bool selectSubApp(int subAppIndex) override;
  int selectedSubApp() const override { return snapshot()->selectedSubApp(); }
  int numberOfSubApps() const override { return static_cast<int>(Snapshot::SubApp::NumberOfSubApps); }

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }
  void didBecomeActive(Escher::Window * window) override;
  Snapshot * snapshot() const { return static_cast<Snapshot *>(Escher::App::snapshot()); }
  EquationStore * equationStore() { return snapshot()->equationStore(); }
  Escher::InputViewController * inputViewController() { return &m_inputViewController; }
  Escher::ViewController * solutionsControllerStack() { return &m_alternateEmptyViewController; }
  Escher::ViewController * intervalController() { return &m_intervalController; }
  SolutionsController * solutionsController() { return &m_solutionsController; }

  TELEMETRY_ID("Solver");

private:
  App(Snapshot * snapshot);
  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;

  // Controllers
  SolutionsController m_solutionsController;
  IntervalController m_intervalController;
  Escher::AlternateEmptyViewController m_alternateEmptyViewController;
  ListController m_listController;
  Escher::ButtonRowController m_listFooter;
  FinanceData m_financeData;
  FinanceResultController m_financeResultController;
  InterestController m_interestController;
  InterestMenuController m_interestMenuController;
  FinanceMenuController m_financeMenuController;
  Shared::MenuController m_menuController;
  Escher::StackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
};

}  // namespace Solver

#endif /* SOLVER_APP_H */
