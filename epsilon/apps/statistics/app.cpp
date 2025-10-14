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
    : m_alternateDataTypeController(&m_storeStackViewController, app(),
                                    {&m_variant.quantitative.m_storeHeader,
                                     &m_variant.categorical.m_storeHeader}),
      m_dataTypeController(&m_storeStackViewController,
                           &m_storeStackViewController,
                           app()->snapshot()->dataTypeViewModel()),
      m_storeStackViewController(
          &app()->m_tabViewController, &m_alternateDataTypeController,
          Escher::StackViewController::Style::WhiteUniform) {
  switchActiveVariant(
      app()->snapshot()->dataTypeViewModel()->selectedDataType(), false);
}

void App::StoreTab::switchActiveVariant(DataTypeViewModel::DataType type,
                                        bool destroy) {
  if (destroy) {
    /* The dataType is fetch from the App direclty in the destructor, it's
     * expected that the viewModel still holds the old dataType */
    assert(App::app()->snapshot()->dataTypeViewModel()->selectedDataType() !=
           type);
    m_variant.~ControllersVariant();
  }
  switch (type) {
    case DataTypeViewModel::DataType::Quantitative: {
      new (&m_variant.quantitative.m_storeController) StoreController(
          &m_variant.quantitative.m_storeHeader, &app()->m_store,
          &m_variant.quantitative.m_storeHeader, &m_storeStackViewController,
          &m_dataTypeController);
      new (&m_variant.quantitative.m_storeHeader) Escher::ButtonRowController(
          &m_alternateDataTypeController,
          &m_variant.quantitative.m_storeController,
          &m_variant.quantitative.m_storeController);
      m_variant.quantitative.m_storeController
          .updateMemoizedFormulasOfErasedSeries();
      break;
    }
    case DataTypeViewModel::DataType::Qualitative: {
      new (&m_variant.categorical.m_storeController)
          Categorical::StoreController(
              &m_variant.categorical.m_storeHeader,
              &m_variant.categorical.m_storeHeader, &m_storeStackViewController,
              &app()->m_tabViewController, &m_dataTypeController,
              &app()->m_categoricalStore);
      new (&m_variant.categorical.m_storeHeader)
          Escher::ButtonRowController(&m_alternateDataTypeController,
                                      &m_variant.categorical.m_storeController,
                                      &m_variant.categorical.m_storeController);
      break;
    }
    default:
      OMG::unreachable();
  }
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
      m_histogramMainController(
          &m_histogramHeader, &m_histogramHeader, &app()->m_tabViewController,
          &m_graphMenuStackViewController, &m_graphTypeController,
          &app()->m_store, app()->snapshot()->storeVersion()),
      m_histogramHeader(&m_graphController, &m_histogramMainController,
                        &m_histogramMainController),
      m_graphTypeController(&m_graphMenuStackViewController,
                            &app()->m_tabViewController,
                            &m_graphMenuStackViewController, &app()->m_store,
                            app()->snapshot()->graphViewModel()),
      m_graphController(&m_graphMenuStackViewController, &m_graphTypeController,
                        {&m_histogramHeader, &m_boxHeader, &m_frequencyHeader,
                         &m_normalProbabilityHeader}),
      // Categorical
      m_barGraphController(
          &m_barGraphHeader, &m_barGraphHeader, &app()->m_tabViewController,
          &m_graphMenuStackViewController, &m_categoricalGraphTypeController),
      m_barGraphHeader(&m_graphController, &m_barGraphController,
                       &m_barGraphController),
      m_pieGraphController(
          &m_pieGraphHeader, &m_pieGraphHeader, &app()->m_tabViewController,
          &m_graphMenuStackViewController, &m_categoricalGraphTypeController),
      m_pieGraphHeader(&m_graphController, &m_pieGraphController,
                       &m_pieGraphController),
      m_categoricalGraphTypeController(
          &m_graphMenuStackViewController, &app()->m_tabViewController,
          &m_graphMenuStackViewController,
          app()->snapshot()->categoricalGraphViewModel()),
      m_categoricalGraphController(&m_graphMenuStackViewController,
                                   &m_categoricalGraphTypeController,
                                   {&m_barGraphHeader, &m_pieGraphHeader}),
      m_alternateDataTypeController(
          &m_graphMenuStackViewController, app(),
          {&m_graphController, &m_categoricalGraphController}),
      m_graphMenuStackViewController(
          &m_graphMenuAlternateEmptyViewController,
          &m_alternateDataTypeController,
          Escher::StackViewController::Style::WhiteUniform),
      m_graphMenuAlternateEmptyViewController(
          &app()->m_tabViewController, &m_graphMenuStackViewController, app()) {
}

App::CalculationTab::CalculationTab()
    : m_calculationController(&m_calculationAlternateEmptyViewController,
                              &m_calculationHeader, &app()->m_store,
                              app()->snapshot()->dataTypeViewModel()),
      m_calculationAlternateEmptyViewController(&m_calculationHeader,
                                                &m_calculationController,
                                                &m_calculationController),
      m_calculationHeader(&app()->m_tabViewController,
                          &m_calculationAlternateEmptyViewController,
                          &m_calculationController) {}

App::App(Snapshot* snapshot)
    : StoreApp(snapshot, &m_inputViewController),
      m_store(snapshot->userPreferences()),
      m_categoricalStore(snapshot->categoricalTableData()),
      m_inputViewController(&m_modalViewController, &m_tabViewController,
                            MathLayoutFieldDelegate::Default()),
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

void App::didBecomeActive(Escher::Window* windows) {
  // Sorted indexes are not kept in the snapshot, they have been invalidated.
  m_store.invalidateSortedIndexes();
  SharedApp::didBecomeActive(windows);
}

}  // namespace Statistics
