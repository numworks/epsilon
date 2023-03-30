#include "app.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/i18n.h>

#include <array>

#include "stat_icon.h"

using namespace Shared;
using namespace Escher;

namespace Statistics {

I18n::Message App::Descriptor::name() const { return I18n::Message::StatsApp; }

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::StatsAppCapital;
}

const Image *App::Descriptor::icon() const { return ImageStore::StatIcon; }

App::Snapshot::Snapshot() : m_storeVersion(0) {
  // Register V1, V2, V3, N1, N2, N3 as reserved names to the sharedStorage.
  static_assert(std::size(DoublePairStore::k_statisticsColumNames) == 2,
                "Number of reserved lists in statistics changed.");
  Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier()
      ->registerArrayOfReservedNames(
          DoublePairStore::k_statisticsColumNames, Ion::Storage::lisExtension,
          Shared::DoublePairStore::k_numberOfSeries,
          std::size(DoublePairStore::k_statisticsColumNames));
}

App *App::Snapshot::unpack(Container *container) {
  return new (container->currentAppBuffer())
      App(this, static_cast<AppsContainer *>(container)->globalContext());
}

void App::Snapshot::reset() {
  m_storeVersion = 0;
  m_graphViewModel.selectGraphView(GraphViewModel::GraphView::Histogram);
  setActiveTab(0);
  m_userPreferences = UserPreferences();
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor *App::Snapshot::descriptor() const {
  return &sDescriptor;
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
    : m_normalProbabilityController(
          &m_normalProbabilityAlternateEmptyViewController,
          &m_normalProbabilityHeader, &app()->m_tabViewController,
          &m_graphMenuStackViewController, &m_graphTypeController,
          &app()->m_store),
      m_normalProbabilityAlternateEmptyViewController(
          &m_normalProbabilityHeader, &m_normalProbabilityController,
          &m_normalProbabilityController),
      m_normalProbabilityHeader(
          &m_graphController, &m_normalProbabilityAlternateEmptyViewController,
          &m_normalProbabilityController),
      m_frequencyController(&m_frequencyHeader, &m_frequencyHeader,
                            &app()->m_tabViewController,
                            &m_graphMenuStackViewController,
                            &m_graphTypeController, &app()->m_store),
      m_frequencyHeader(&m_graphController, &m_frequencyController,
                        &m_frequencyController),
      m_boxController(&m_boxHeader, &m_boxHeader, &app()->m_tabViewController,
                      &m_graphMenuStackViewController, &m_graphTypeController,
                      &app()->m_store),
      m_boxHeader(&m_graphController, &m_boxController, &m_boxController),
      m_histogramController(&m_histogramHeader, app(), &m_histogramHeader,
                            &app()->m_tabViewController,
                            &m_graphMenuStackViewController,
                            &m_graphTypeController, &app()->m_store,
                            app()->snapshot()->storeVersion()),
      m_histogramHeader(&m_graphController, &m_histogramController,
                        &m_histogramController),
      m_graphTypeController(&m_graphMenuStackViewController,
                            &app()->m_tabViewController,
                            &m_graphMenuStackViewController, &app()->m_store,
                            app()->snapshot()->graphViewModel()),
      m_graphController(&m_graphMenuStackViewController, app(),
                        {&m_histogramHeader, &m_boxHeader, &m_frequencyHeader,
                         &m_normalProbabilityHeader}),
      m_graphMenuStackViewController(
          &m_graphMenuAlternateEmptyViewController, &m_graphController,
          Escher::StackViewController::Style::WhiteUniform),
      m_graphMenuAlternateEmptyViewController(&app()->m_tabViewController,
                                              &m_graphMenuStackViewController,
                                              &m_graphTypeController) {}

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
              snapshot->userPreferences()),
      m_context(parentContext),
      m_inputViewController(&m_modalViewController, &m_tabViewController, this,
                            this),
      m_tabViewController(&m_inputViewController, snapshot, &m_tabs) {
  // Order used in m_graphController constructor
  assert(GraphViewModel::IndexOfGraphView(
             GraphViewModel::GraphView::Histogram) == 0);
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Box) == 1);
  assert(GraphViewModel::IndexOfGraphView(
             GraphViewModel::GraphView::Frequency) == 2);
  assert(GraphViewModel::IndexOfGraphView(
             GraphViewModel::GraphView::NormalProbability) == 3);
}

void App::activeViewDidBecomeFirstResponder(
    Escher::ViewController *activeViewController) {
  if (m_store.graphViewHasBeenInvalidated()) {
    m_tabs.tab<GraphTab>()->m_graphMenuStackViewController.push(
        &m_tabs.tab<GraphTab>()->m_graphTypeController);
  } else {
    setFirstResponder(activeViewController);
  }
}

void App::didBecomeActive(Escher::Window *windows) {
  // Sorted indexes are not kept in the snapshot, they have been invalidated.
  m_store.invalidateSortedIndexes();
  ExpressionFieldDelegateApp::didBecomeActive(windows);
  m_tabViewController.enterActiveTab();
}

}  // namespace Statistics
