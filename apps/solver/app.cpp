#include "app.h"

#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <poincare/comparison.h>
#include <poincare/preferences.h>

#include "solver_icon.h"

using namespace Shared;
using namespace Escher;

namespace Solver {

const Image* App::Descriptor::icon() const { return ImageStore::SolverIcon; }

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::reset() {
  // Delete all equations
  m_equationStore.removeAll();
  Shared::SharedApp::Snapshot::reset();
}

void App::prepareForIntrusiveStorageChange() {
  LayoutFieldDelegateApp::prepareForIntrusiveStorageChange();
  m_stackViewController.popUntilDepth(1, true);
}

void App::storageDidChangeForRecord(Ion::Storage::Record record) {
  equationStore()->storageDidChangeForRecord(record);
}

void App::Snapshot::tidy() {
  // Delete all expressions of equations
  m_equationStore.tidyDownstreamPoolFrom();
  SharedApp::Snapshot::tidy();
}

App::App(Snapshot* snapshot)
    : LayoutFieldDelegateApp(snapshot, &m_stackViewController),
      m_solutionsController(&m_alternateEmptyViewController),
      m_intervalController(nullptr, this),
      m_alternateEmptyViewController(nullptr, &m_solutionsController,
                                     &m_solutionsController),
      m_listController(&m_listFooter, this, snapshot->equationStore(),
                       &m_listFooter),
      m_listFooter(&m_stackViewController, &m_listController, &m_listController,
                   ButtonRowController::Position::Bottom,
                   ButtonRowController::Style::EmbossedGray,
                   ButtonRowController::Size::Large),
      m_stackViewController(&m_modalViewController, &m_listFooter,
                            StackViewController::Style::GrayGradation),
      m_system(snapshot->equationStore()),
      m_context(AppsContainer::sharedAppsContainer()->globalContext()) {}

bool App::isAcceptableExpression(EditableField* field,
                                 const Poincare::Expression exp) {
  /* Complete LayoutFieldDelegateApp acceptable conditions by only accepting
   * the Equal OperatorType in the list of equations. */
  return LayoutFieldDelegateApp::isAcceptableExpression(field, exp) &&
         (field != m_listController.layoutField() ||
          Poincare::ComparisonNode::IsBinaryEquality(exp));
}

}  // namespace Solver
