#include "app.h"

#include <apps/apps_container.h>
#include <apps/i18n.h>

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
  EquationStore::RemoveAll();
  Shared::SharedApp::Snapshot::reset();
}

void App::prepareForIntrusiveStorageChange() {
  MathApp::prepareForIntrusiveStorageChange();
  m_stackViewController.popUntilDepth(1, true);
}

void App::storageDidChangeForRecord(Ion::Storage::Record record) {
  equationStore()->storageDidChangeForRecord(record);
}

void App::openSolutionsController(bool approximateSolve) {
  if (!approximateSolve) {
    m_stackViewController.push(&m_solutionsController);
    return;
  }
  // stackViewController.push clears parentResponder, it must be reset.
  m_solutionsController.setParentResponder(&m_solutionHeader);
  m_stackViewController.push(&m_solutionHeader);
}

void App::openIntervalController() {
  m_stackViewController.push(&m_intervalController);
}

App::App(Snapshot* snapshot)
    : MathApp(snapshot, &m_stackViewController),
      m_solutionsController(&m_stackViewController, &m_solutionHeader),
      m_solutionHeader(
          &m_stackViewController, &m_solutionsController,
          &m_solutionsController, ButtonRowController::Position::Top,
          ButtonRowController::Style::PlainWhite,
          ButtonRowController::Size::Small, Metric::CommonSmallMargin),
      m_intervalController(nullptr),
      m_listController(&m_listFooter, &m_equationStore, &m_listFooter),
      m_listFooter(&m_stackViewController, &m_listController, &m_listController,
                   ButtonRowController::Position::Bottom,
                   ButtonRowController::Style::EmbossedGray,
                   ButtonRowController::Size::Large),
      m_stackViewController(&m_modalViewController, &m_listFooter,
                            StackViewController::Style::GrayGradation),
      m_system(&m_equationStore),
      m_context(AppsContainer::sharedAppsContainer()->globalContext()) {}

}  // namespace Solver
