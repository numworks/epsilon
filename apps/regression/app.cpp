#include "app.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/i18n.h>

#include <array>

#include "regression_icon.h"

using namespace Shared;
using namespace Escher;

namespace Regression {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::RegressionApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::RegressionAppCapital;
}

const Image *App::Descriptor::icon() const {
  return ImageStore::RegressionIcon;
}

App::Snapshot::Snapshot()
    : m_graphSelectedDotIndex(-1),
      m_selectedCurveIndex(-1),
      m_regressionTypes{Model::Type::None, Model::Type::None,
                        Model::Type::None} {
  // Register X1, X2, X3, Y1, Y2, Y3 as reserved lists to the sharedStorage.
  static_assert(std::size(DoublePairStore::k_regressionColumNames) == 2,
                "Number of reserved lists in regression changed.");
  Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier()
      ->registerArrayOfReservedNames(
          DoublePairStore::k_regressionColumNames, Ion::Storage::lisExtension,
          Shared::DoublePairStore::k_numberOfSeries,
          std::size(DoublePairStore::k_regressionColumNames));

  // Register R1, R2, and R3 as reserved functions to the sharedStorage.
  Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier()
      ->registerArrayOfReservedNames(
          &Store::k_functionName, Ion::Storage::regExtension,
          Shared::DoublePairStore::k_numberOfSeries, 1);
}

App *App::Snapshot::unpack(Container *container) {
  return new (container->currentAppBuffer())
      App(this, static_cast<AppsContainer *>(container)->globalContext());
}

void App::Snapshot::reset() {
  setActiveTab(0);
  memset(m_regressionTypes, 0, sizeof(m_regressionTypes));
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor *App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::didBecomeActive(Escher::Window *window) {
  // Delay loadMemoizedFormulasFromSnapshot to tab activation
  ExpressionFieldDelegateApp::didBecomeActive(window);
}

App::StoreTab::StoreTab()
    : m_storeController(&m_storeHeader, app(), &app()->m_store, &m_storeHeader,
                        app()->m_context),
      m_storeHeader(&m_storeStackViewController, &m_storeController,
                    &m_storeController),
      m_storeStackViewController(
          &app()->m_tabViewController, &m_storeHeader,
          Escher::StackViewController::Style::WhiteUniform) {
  m_storeController.loadMemoizedFormulasFromSnapshot();
}

App::GraphTab::GraphTab()
    : m_graphController(
          &m_graphAlternateEmptyViewController, app(), &m_graphHeader,
          app()->snapshot()->graphRange(), app()->snapshot()->cursor(),
          app()->snapshot()->graphSelectedDotIndex(),
          app()->snapshot()->selectedCurveIndex(), &app()->m_store),
      m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController,
                                          &m_graphController),
      m_graphHeader(&m_graphStackViewController,
                    &m_graphAlternateEmptyViewController, &m_graphController),
      m_graphStackViewController(
          &app()->m_tabViewController, &m_graphHeader,
          Escher::StackViewController::Style::WhiteUniform) {}

App::CalculationTab::CalculationTab()
    : m_calculationController(&m_calculationAlternateEmptyViewController,
                              &m_calculationHeader, &app()->m_store),
      m_calculationAlternateEmptyViewController(&m_calculationHeader,
                                                &m_calculationController,
                                                &m_calculationController),
      m_calculationHeader(&app()->m_tabViewController,
                          &m_calculationAlternateEmptyViewController,
                          &m_calculationController) {}

App::App(Snapshot *snapshot, Poincare::Context *parentContext)
    : StoreApp(snapshot, &m_inputViewController),
      m_store(AppsContainerHelper::sharedAppsContainerGlobalContext(),
              snapshot->storePreferences(), snapshot->regressionTypes()),
      m_regressionController(nullptr, &m_store),
      m_inputViewController(&m_modalViewController, &m_tabViewController, this,
                            this),
      m_context(parentContext),
      m_tabViewController(&m_inputViewController, snapshot, this, &m_tabs,
                          I18n::Message::DataTab, I18n::Message::GraphTab,
                          I18n::Message::StatTab) {}

}  // namespace Regression
