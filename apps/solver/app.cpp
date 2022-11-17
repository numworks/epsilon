#include "app.h"
#include <apps/i18n.h>
#include <apps/apps_container.h>
#include <apps/exam_mode_configuration.h>
#include "solver_icon.h"
#include <poincare/comparison.h>
#include <poincare/preferences.h>

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
}

void App::storageDidChangeForRecord(Ion::Storage::Record record) {
  equationStore()->storageDidChangeForRecord(record);
  if (record.isNull()) {
    return;
  }
  m_stackViewController.popUntilDepth(1, true);
}

void App::Snapshot::tidy() {
  // Delete all expressions of equations
  m_equationStore.tidyDownstreamPoolFrom();
}

App::App(Snapshot * snapshot) :
  ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
  m_solutionsController(&m_alternateEmptyViewController, snapshot->equationStore()),
  m_intervalController(nullptr, this, snapshot->equationStore()),
  m_alternateEmptyViewController(nullptr, &m_solutionsController, &m_solutionsController),
  m_listController(&m_listFooter, snapshot->equationStore(), &m_listFooter),
  m_listFooter(&m_stackViewController, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray, ButtonRowController::Size::Large),
  m_stackViewController(&m_inputViewController, &m_listFooter, StackViewController::Style::GrayGradation),
  m_inputViewController(&m_modalViewController, &m_stackViewController, this, &m_listController, &m_listController),
  m_context(AppsContainer::sharedAppsContainer()->globalContext())
{}

bool App::isAcceptableExpression(const Poincare::Expression exp) {
  /* Complete ExpressionFieldDelegateApp acceptable conditions by only accepting
   * the Equal OperatorType. */
  return ExpressionFieldDelegateApp::isAcceptableExpression(exp)
         && (Poincare::ComparisonNode::IsBinaryEquality(exp)
            || exp.type() != Poincare::ExpressionNode::Type::Comparison);
}

}
