#include "app.h"
#include "stat_icon.h"
#include "../apps_container.h"
#include <apps/i18n.h>

using namespace Shared;
using namespace Escher;

namespace Statistics {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::StatsApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::StatsAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::StatIcon;
}

App::Snapshot::Snapshot() :
  m_storeVersion(0)
{
  // Register V1, V2, V3, N1, N2, N3 as reserved names to the sharedStorage.
  Ion::Storage::FileSystem::sharedFileSystem()->recordNameVerifier()->registerArrayOfReservedNames(Store::k_columnNames, Ion::Storage::lisExtension, Shared::DoublePairStore::k_numberOfSeries, sizeof(Store::k_columnNames) / sizeof(char *));
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this, static_cast<AppsContainer *>(container)->globalContext());
}

void App::Snapshot::reset() {
  m_storeVersion = 0;
  m_graphViewModel.selectGraphView(GraphViewModel::GraphView::Histogram);
  setActiveTab(0);
  m_userPreferences = UserPreferences();
}

static constexpr App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot * snapshot, Poincare::Context * parentContext) :
  ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
  m_store(AppsContainer::sharedAppsContainer()->globalContext(), snapshot->userPreferences()),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_normalProbabilityController(&m_normalProbabilityAlternateEmptyViewController, &m_normalProbabilityHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, &m_store),
  m_normalProbabilityAlternateEmptyViewController(&m_normalProbabilityHeader, &m_normalProbabilityController, &m_normalProbabilityController),
  m_normalProbabilityHeader(&m_graphController, &m_normalProbabilityAlternateEmptyViewController, &m_normalProbabilityController),
  m_frequencyController(&m_frequencyHeader, &m_frequencyHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, &m_store),
  m_frequencyHeader(&m_graphController, &m_frequencyController, &m_frequencyController),
  m_boxController(&m_boxHeader, &m_boxHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, &m_store),
  m_boxHeader(&m_graphController, &m_boxController, &m_boxController),
  m_histogramController(&m_histogramHeader, this, &m_histogramHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, &m_store, snapshot->storeVersion()),
  m_histogramHeader(&m_graphController, &m_histogramController, &m_histogramController),
  m_graphTypeController(&m_graphMenuStackViewController, &m_tabViewController, &m_graphMenuStackViewController, &m_store, snapshot->graphViewModel()),
  m_graphController(&m_graphMenuStackViewController, this, {&m_histogramHeader, &m_boxHeader, &m_frequencyHeader, &m_normalProbabilityHeader}),
  m_graphMenuStackViewController(&m_graphMenuAlternateEmptyViewController, &m_graphController, Escher::StackViewController::Style::WhiteUniform),
  m_graphMenuAlternateEmptyViewController(&m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController),
  m_storeController(&m_storeHeader, this, &m_store, &m_storeHeader, parentContext),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader, Escher::StackViewController::Style::WhiteUniform),
  m_tabViewController(&m_inputViewController, snapshot, &m_storeStackViewController, &m_graphMenuAlternateEmptyViewController, &m_calculationHeader),
  m_inputViewController(&m_modalViewController, &m_tabViewController, &m_storeController, &m_storeController, &m_storeController)
{
  // Order used in m_graphController constructor
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Histogram) == 0);
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Box) == 1);
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Frequency) == 2);
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::NormalProbability) == 3);
}

void App::activeViewDidBecomeFirstResponder(Escher::ViewController * activeViewController) {
  if (m_store.graphViewHasBeenInvalidated()) {
    m_graphMenuStackViewController.push(&m_graphTypeController);
  } else {
    setFirstResponder(activeViewController);
  }
}

void App::didBecomeActive(Escher::Window * windows) {
  // Sorted indexes are not kept in the snapshot, they have been invalidated.
  m_store.invalidateSortedIndexes();
  Escher::App::didBecomeActive(windows);
}

}
