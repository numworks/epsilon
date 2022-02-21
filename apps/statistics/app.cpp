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
  m_selectedGraphViewIndex(0),
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
  m_selectedGraphViewIndex = 0;
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
  m_boxController(&m_boxHeader, &m_boxHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, snapshot->store(), snapshot->selectedBoxQuantile(), snapshot->selectedBoxSeriesIndex()),
  m_boxHeader(&m_graphController, &m_boxController, &m_boxController),
  m_histogramController(&m_histogramHeader, this, &m_histogramHeader, &m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController, snapshot->store(), snapshot->storeVersion(), snapshot->barVersion(), snapshot->rangeVersion(), snapshot->selectedHistogramBarIndex(), snapshot->selectedHistogramSeriesIndex()),
  m_histogramHeader(&m_graphController, &m_histogramController, &m_histogramController),
  m_graphTypeController(&m_graphMenuStackViewController, &m_tabViewController, &m_graphMenuStackViewController, snapshot->store(), snapshot->selectedGraphViewIndex()),
  m_graphController(&m_graphMenuStackViewController, this, m_graphControllerViews, I18n::translate(I18n::Message::GraphTab)),
  m_graphMenuStackViewController(&m_graphMenuAlternateEmptyViewController, &m_graphController, Escher::StackViewController::Style::WhiteUniform),
  m_graphMenuAlternateEmptyViewController(&m_tabViewController, &m_graphMenuStackViewController, &m_graphTypeController),
  m_storeController(&m_storeStackViewController, this, snapshot->store(), &m_storeHeader, parentContext),
  m_storeStackViewController(&m_storeHeader, &m_storeController, Escher::StackViewController::Style::WhiteUniform),
  m_storeHeader(&m_tabViewController, &m_storeStackViewController, &m_storeController),
  m_tabViewController(&m_modalViewController, snapshot, &m_storeHeader, &m_graphMenuAlternateEmptyViewController, &m_calculationHeader)
{
  m_graphControllerViews[0] = &m_histogramHeader;
  m_graphControllerViews[1] = &m_boxHeader;
}

int App::activeViewControllerIndex() const {
  assert(*snapshot()->selectedGraphViewIndex() >= 0 && *snapshot()->selectedGraphViewIndex() < 4);
  return *snapshot()->selectedGraphViewIndex();
}

void App::didBecomeActive(Escher::Window * windows) {
  // TODO : Only push after reset or when data is deleted
  m_graphMenuStackViewController.push(&m_graphTypeController);
  Escher::App::didBecomeActive(windows);
}

}
