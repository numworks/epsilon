#include "app.h"
#include <apps/i18n.h>
#include <apps/apps_container.h>
#include <apps/exam_mode_configuration.h>
#include "solver_icon.h"
#include <poincare/comparison_operator.h>
#include <poincare/preferences.h>
#include "equations_icon.h"
#include "finance_icon.h"

using namespace Shared;
using namespace Escher;

namespace Solver {

const Image * App::Descriptor::icon() const {
  return ImageStore::SolverIcon;
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::reset() {
  // Delete all equations
  m_equationStore.removeAll();
  m_activeSubapp = SubApp::Unknown;
}

void App::Snapshot::storageDidChangeForRecord(const Ion::Storage::Record record) {
  m_equationStore.storageDidChangeForRecord(record);
}

void App::Snapshot::tidy() {
  // Delete all expressions of equations
  m_equationStore.tidyDownstreamPoolFrom();
}

bool App::selectSubApp(int subAppIndex) {
  if (Poincare::Preferences::sharedPreferences()->equationSolverIsForbidden() && static_cast<Snapshot::SubApp>(subAppIndex) == Snapshot::SubApp::Equation) {
    displayWarning(I18n::Message::DisabledFeatureInTestMode1, I18n::Message::DisabledFeatureInTestMode2);
    return false;
  }
  snapshot()->selectSubApp(subAppIndex);
  return true;
}

App::App(Snapshot * snapshot) :
  ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
  m_solutionsController(&m_alternateEmptyViewController, snapshot->equationStore()),
  m_intervalController(nullptr, this, snapshot->equationStore()),
  m_alternateEmptyViewController(nullptr, &m_solutionsController, &m_solutionsController),
  m_listController(&m_listFooter, snapshot->equationStore(), &m_listFooter),
  m_listFooter(&m_stackViewController, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray, ButtonRowController::Size::Large),
  m_financeResultController(&m_stackViewController, m_financeData.interestData()),
  m_interestController(&m_stackViewController, &m_inputViewController, &m_financeResultController, m_financeData.interestData()),
  m_interestMenuController(&m_stackViewController, &m_interestController, m_financeData.interestData()),
  m_financeMenuController(&m_stackViewController, &m_interestMenuController, m_financeData.interestData()),
  m_menuController(
      &m_stackViewController,
      {&m_listFooter, &m_financeMenuController},
      {{I18n::Message::EquationsSubAppTitle, I18n::Message::EquationsSubAppDescription}, {I18n::Message::FinanceSubAppTitle, I18n::Message::FinanceSubAppDescription}},
      {ImageStore::EquationsIcon, ImageStore::FinanceIcon},
      this
    ),
  m_stackViewController(&m_inputViewController, &m_menuController, StackViewController::Style::GrayGradation),
  m_inputViewController(&m_modalViewController, &m_stackViewController, this, &m_listController, &m_listController),
  m_context(AppsContainer::sharedAppsContainer()->globalContext())
{}

void App::didBecomeActive(Escher::Window * windows) {
  // If selectedSubApp is known, directly open the selectedSubApp controller
  if (snapshot()->selectedSubApp() == static_cast<int>(Snapshot::SubApp::Equation)) {
    m_menuController.stackOpenPage(&m_listFooter);
  } else if (snapshot()->selectedSubApp() == static_cast<int>(Snapshot::SubApp::Finance)) {
    m_menuController.stackOpenPage(&m_financeMenuController);
  }
  Escher::App::didBecomeActive(windows);
}

bool App::isAcceptableExpression(const Poincare::Expression exp) {
  /* Complete ExpressionFieldDelegateApp acceptable conditions by only accepting
   * the Equal OperatorType. */
  return ExpressionFieldDelegateApp::isAcceptableExpression(exp) && (
    exp.type() == Poincare::ExpressionNode::Type::Equal
    || !Poincare::ComparisonOperator::IsComparisonOperatorType(exp.type()));
}

}
