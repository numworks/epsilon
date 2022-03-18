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
  m_storeVersion(0),
  m_barVersion(0),
  m_rangeVersion(0),
  m_selectedHistogramSeriesIndex(-1),
  m_selectedHistogramBarIndex(0),
  m_selectedBoxSeriesIndex(-1),
  m_selectedBoxQuantile(BoxView::Quantile::Min)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this, static_cast<AppsContainer *>(container)->globalContext());
}

void App::Snapshot::reset() {
  m_store.deleteAllPairs();
  m_storeVersion = 0;
  m_barVersion = 0;
  m_rangeVersion = 0;
  m_graphViewModel.selectGraphView(GraphViewModel::GraphView::Histogram);
  m_selectedHistogramBarIndex = 0;
  m_selectedBoxQuantile = BoxView::Quantile::Min;
  setActiveTab(0);
}

static constexpr App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot * snapshot, Poincare::Context * parentContext) :
  TextFieldDelegateApp(snapshot, &m_tabViewController),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, snapshot->store()),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_normalProbabilityController(&m_normalProbabilityHeader, &m_normalProbabilityHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, snapshot->store(), snapshot->selectedHistogramBarIndex(), snapshot->selectedBoxSeriesIndex()),
  m_normalProbabilityHeader(&m_graphController, &m_normalProbabilityController, &m_normalProbabilityController),
  m_frequencyController(&m_frequencyHeader, &m_frequencyHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, snapshot->store(), snapshot->selectedHistogramBarIndex(), snapshot->selectedBoxSeriesIndex()),
  m_frequencyHeader(&m_graphController, &m_frequencyController, &m_frequencyController),
  m_boxController(&m_boxHeader, &m_boxHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, snapshot->store(), snapshot->selectedBoxQuantile(), snapshot->selectedBoxSeriesIndex()),
  m_boxHeader(&m_graphController, &m_boxController, &m_boxController),
  m_histogramController(&m_histogramHeader, this, &m_histogramHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, snapshot->store(), snapshot->storeVersion(), snapshot->barVersion(), snapshot->rangeVersion(), snapshot->selectedHistogramBarIndex(), snapshot->selectedHistogramSeriesIndex()),
  m_histogramHeader(&m_graphController, &m_histogramController, &m_histogramController),
  m_graphTypeController(&m_graphMenuStackViewController, &m_tabViewController, &m_graphMenuStackViewController, snapshot->store(), snapshot->graphViewModel()),
  m_graphController(&m_graphMenuStackViewController, this, {&m_histogramHeader, &m_boxHeader, &m_frequencyHeader, &m_normalProbabilityHeader}),
  m_graphMenuStackViewController(&m_graphMenuAlternateEmptyViewController, &m_graphController, Escher::StackViewController::Style::WhiteUniform),
  m_graphMenuAlternateEmptyViewController(&m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController),
  m_storeController(&m_storeHeader, this, snapshot->store(), &m_storeHeader, parentContext),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader, Escher::StackViewController::Style::WhiteUniform),
  m_tabViewController(&m_modalViewController, snapshot, &m_storeStackViewController, &m_graphMenuAlternateEmptyViewController, &m_calculationHeader)
{
  /* Set the store's sorted index buffer. It is stored in the app to reduce
   * snapshot's size. */
  snapshot->store()->setSortedIndex(m_sortedIndexBuffer, k_sortedIndexBufferSize);

  // Order used in m_graphController constructor
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Histogram) == 0);
  assert(GraphViewModel::IndexOfGraphView(GraphViewModel::GraphView::Box) == 1);
}

void App::didBecomeActive(Escher::Window * windows) {
  // TODO : Only push after reset or when data is deleted
  m_graphMenuStackViewController.push(&m_graphTypeController);
  Escher::App::didBecomeActive(windows);
}

}
