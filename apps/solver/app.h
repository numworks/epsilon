#ifndef SOLVER_SOLVER_APP_H
#define SOLVER_SOLVER_APP_H

#include <escher.h>
#include "../shared/expression_field_delegate_app.h"
#include "list_controller.h"
#include "equation_store.h"
#include "interval_controller.h"
#include "solutions_controller.h"

namespace Solver {

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
    Snapshot();
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
    void reset() override;
    EquationStore * equationStore() { return &m_equationStore; }
  private:
    void tidy() override;
    EquationStore m_equationStore;
  };
  InputViewController * inputViewController() { return &m_inputViewController; }
  ViewController * solutionsControllerStack() { return &m_alternateEmptyViewController; }
  ViewController * intervalController() { return &m_intervalController; }
  void willBecomeInactive() override;
  char XNT() override;
private:
  App(Snapshot * snapshot);
  SolutionsController m_solutionsController;
  IntervalController m_intervalController;
  AlternateEmptyViewController m_alternateEmptyViewController;
  ListController m_listController;
  ButtonRowController m_listFooter;
  StackViewController m_stackViewController;
  InputViewController m_inputViewController;
};

inline App * app() {
  return static_cast<App *>(Container::activeApp());
}

}

#endif
