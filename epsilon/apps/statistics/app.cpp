#include "app.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <ion/storage/record.h>

#include <array>

#include "shared/global_store.h"
#include "stat_icon.h"

using namespace Shared;
using namespace Escher;

namespace Statistics {

I18n::Message App::Descriptor::name() const { return I18n::Message::StatsApp; }

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::StatsAppCapital;
}

const Image* App::Descriptor::icon() const { return ImageStore::StatIcon; }

App::Snapshot::Snapshot()
    : m_storeVersion(0),
      m_selectedSeries(-1),
      m_selectedIndex(DataView::k_defaultSelectedIndex) {
  /* Register V1, V2, V3, ..., N1, N2, N3, ... as reserved names to the
   * sharedStorage. */
  static_assert(std::size(DoublePairStore::k_statisticsColumNames) == 2,
                "Number of reserved lists in statistics changed.");
  Ion::Storage::FileSystem::sharedFileSystem->recordNameVerifier()
      ->registerArrayOfReservedNames(
          DoublePairStore::k_statisticsColumNames, Ion::Storage::listExtension,
          Shared::DoublePairStore::k_numberOfSeries,
          std::size(DoublePairStore::k_statisticsColumNames));
}

App* App::Snapshot::unpack(Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  m_storeVersion = 0;
  m_graphViewModel.selectGraphView(GraphViewModel::GraphView::Histogram);
  setActiveTab(0);
  m_userPreferences = UserPreferences();
  Shared::SharedApp::Snapshot::reset();
}

void App::Snapshot::countryWasUpdated() {
  m_userPreferences.setDisplayOutliers(
      GlobalPreferences::SharedGlobalPreferences()->outliersStatus() ==
      CountryPreferences::OutlierDefaultVisibility::Displayed);
}

const char* App::memoizedFormulaExtension() const {
  return Ion::Storage::statisticsMemoizedFormulaExtension;
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::StoreTab::StoreTab()
    : m_storeController(&m_storeHeader, &app()->m_store, &m_storeHeader,
                        &m_storeStackViewController, &m_dataTypeController),
      m_storeHeader(&m_storeStackViewController, &m_storeController,
                    &m_storeController),
      m_dataTypeController(&m_storeStackViewController,
                           &m_storeStackViewController,
                           app()->snapshot()->dataTypeViewModel(),
                           &app()->m_inputViewController, app()->snapshot()),
      m_storeStackViewController(
          &app()->m_quantitativeTabViewController, &m_storeHeader,
          Escher::StackViewController::Style::WhiteUniform) {
  m_storeController.updateMemoizedFormulasOfErasedSeries();
}

App::GraphTab::GraphTab()
    : m_normalProbabilityController(
          &m_normalProbabilityAlternateEmptyViewController,
          &m_normalProbabilityHeader, &app()->m_quantitativeTabViewController,
          &m_graphMenuStackViewController, &m_graphTypeController,
          &app()->m_store),
      m_normalProbabilityAlternateEmptyViewController(
          &m_normalProbabilityHeader, &m_normalProbabilityController,
          &m_normalProbabilityController),
      m_normalProbabilityHeader(
          &m_graphController, &m_normalProbabilityAlternateEmptyViewController,
          &m_normalProbabilityController),
      m_frequencyController(&m_frequencyHeader, &m_frequencyHeader,
                            &app()->m_quantitativeTabViewController,
                            &m_graphMenuStackViewController,
                            &m_graphTypeController, &app()->m_store),
      m_frequencyHeader(&m_graphController, &m_frequencyController,
                        &m_frequencyController),
      m_boxController(&m_boxHeader, &m_boxHeader,
                      &app()->m_quantitativeTabViewController,
                      &m_graphMenuStackViewController, &m_graphTypeController,
                      &app()->m_store),
      m_boxHeader(&m_graphController, &m_boxController, &m_boxController),
      m_histogramMainController(&m_histogramHeader, &m_histogramHeader,
                                &app()->m_quantitativeTabViewController,
                                &m_graphMenuStackViewController,
                                &m_graphTypeController, &app()->m_store,
                                app()->snapshot()->storeVersion()),
      m_histogramHeader(&m_graphController, &m_histogramMainController,
                        &m_histogramMainController),
      m_graphTypeController(&m_graphMenuStackViewController,
                            &app()->m_quantitativeTabViewController,
                            &m_graphMenuStackViewController, &app()->m_store,
                            app()->snapshot()->graphViewModel()),
      m_graphController(&m_graphMenuStackViewController, &m_graphTypeController,
                        {&m_histogramHeader, &m_boxHeader, &m_frequencyHeader,
                         &m_normalProbabilityHeader}),
      m_graphMenuStackViewController(
          &m_graphMenuAlternateEmptyViewController, &m_graphController,
          Escher::StackViewController::Style::WhiteUniform),
      m_graphMenuAlternateEmptyViewController(
          &app()->m_quantitativeTabViewController,
          &m_graphMenuStackViewController, &m_graphTypeController) {}

App::CalculationTab::CalculationTab()
    : m_calculationController(&m_calculationAlternateEmptyViewController,
                              &m_calculationHeader, &app()->m_store),
      m_calculationAlternateEmptyViewController(&m_calculationHeader,
                                                &m_calculationController,
                                                &m_calculationController),
      m_calculationHeader(&app()->m_quantitativeTabViewController,
                          &m_calculationAlternateEmptyViewController,
                          &m_calculationController) {}

App::CategoricalStoreTab::CategoricalStoreTab()
    : m_storeController(
          &m_storeHeader, &m_storeHeader, &m_storeStackViewController,
          &app()->m_categoricalTabViewController, &m_dataTypeController,
          app()->snapshot()->categoricalTableData()),
      m_storeHeader(&m_storeStackViewController, &m_storeController,
                    &m_storeController),
      m_dataTypeController(&m_storeStackViewController,
                           &m_storeStackViewController,
                           app()->snapshot()->dataTypeViewModel(),
                           &app()->m_inputViewController, app()->snapshot()),
      m_storeStackViewController(
          &app()->m_categoricalTabViewController, &m_storeHeader,
          Escher::StackViewController::Style::WhiteUniform) {}

App::CategoricalGraphTab::CategoricalGraphTab()
    : m_barGraphController(&m_barGraphHeader, &m_barGraphHeader,
                           &app()->m_categoricalTabViewController,
                           &m_graphMenuStackViewController,
                           &m_graphTypeController),
      m_barGraphHeader(&m_graphController, &m_barGraphController,
                       &m_barGraphController),
      m_pieGraphController(&m_pieGraphHeader, &m_pieGraphHeader,
                           &app()->m_categoricalTabViewController,
                           &m_graphMenuStackViewController,
                           &m_graphTypeController),
      m_pieGraphHeader(&m_graphController, &m_pieGraphController,
                       &m_pieGraphController),
      m_graphTypeController(&m_graphMenuStackViewController,
                            &app()->m_categoricalTabViewController,
                            &m_graphMenuStackViewController,
                            app()->snapshot()->categoricalGraphViewModel()),
      m_graphController(&m_graphMenuStackViewController, &m_graphTypeController,
                        {&m_barGraphHeader, &m_pieGraphHeader}),
      m_graphMenuStackViewController(
          &m_graphMenuAlternateEmptyViewController, &m_graphController,
          Escher::StackViewController::Style::WhiteUniform),
      m_graphMenuAlternateEmptyViewController(
          &app()->m_categoricalTabViewController,
          &m_graphMenuStackViewController, &m_graphTypeController) {};

App::App(Snapshot* snapshot)
    : StoreApp(snapshot, &m_inputViewController),
      m_store(snapshot->userPreferences()),
      m_inputViewController(&m_modalViewController, &m_alternateViewController,
                            MathLayoutFieldDelegate::Default()),
      m_alternateViewController(
          &m_inputViewController, this,
          {&m_quantitativeTabViewController, &m_categoricalTabViewController}),
      m_quantitativeTabViewController(&m_alternateViewController, snapshot,
                                      &m_quantitativeTabs),
      m_categoricalTabViewController(&m_alternateViewController, snapshot,
                                     &m_categoricalTabs) {
  // Order used in m_graphController constructor
  assert(GraphViewModel::IndexOfGraphView(
             GraphViewModel::GraphView::Histogram) == 0);
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Box) == 1);
  assert(GraphViewModel::IndexOfGraphView(
             GraphViewModel::GraphView::Frequency) == 2);
  assert(GraphViewModel::IndexOfGraphView(
             GraphViewModel::GraphView::NormalProbability) == 3);
}

void App::didBecomeActive(Escher::Window* windows) {
  // Sorted indexes are not kept in the snapshot, they have been invalidated.
  m_store.invalidateSortedIndexes();
  SharedApp::didBecomeActive(windows);
}

}  // namespace Statistics
