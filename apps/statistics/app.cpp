#include "app.h"
#include "stat_icon.h"
#include "../i18n.h"

using namespace Shared;

namespace Statistics {

I18n::Message App::Descriptor::name() {
  return I18n::Message::StatsApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::StatsAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::StatIcon;
}

App::Snapshot::Snapshot() :
  m_store(),
  m_storeVersion(0),
  m_barVersion(0),
  m_rangeVersion(0),
  m_selectedHistogramBarIndex(0),
  m_selectedBoxQuantile(BoxView::Quantile::Min)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_store.deleteAllPairs();
  m_storeVersion = 0;
  m_barVersion = 0;
  m_rangeVersion = 0;
  m_selectedHistogramBarIndex = 0;
  m_selectedBoxQuantile = BoxView::Quantile::Min;
  setActiveTab(0);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

Store * App::Snapshot::store() {
  return &m_store;
}

uint32_t * App::Snapshot::storeVersion() {
  return &m_storeVersion;
}

uint32_t * App::Snapshot::barVersion() {
  return &m_barVersion;
}

uint32_t * App::Snapshot::rangeVersion() {
  return &m_rangeVersion;
}

int * App::Snapshot::selectedHistogramBarIndex() {
  return &m_selectedHistogramBarIndex;
}

BoxView::Quantile * App::Snapshot::selectedBoxQuantile() {
  return &m_selectedBoxQuantile;
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_tabViewController),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, snapshot->store()),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_boxController(&m_boxAlternateEmptyViewController, &m_boxHeader, snapshot->store(), snapshot->selectedBoxQuantile()),
  m_boxAlternateEmptyViewController(&m_boxHeader, &m_boxController, &m_boxController),
  m_boxHeader(&m_tabViewController, &m_boxAlternateEmptyViewController, &m_boxController),
  m_histogramController(&m_histogramAlternateEmptyViewController, &m_histogramHeader, snapshot->store(), snapshot->storeVersion(), snapshot->barVersion(), snapshot->rangeVersion(), snapshot->selectedHistogramBarIndex()),
  m_histogramAlternateEmptyViewController(&m_histogramHeader, &m_histogramController, &m_histogramController),
  m_histogramHeader(&m_histogramStackViewController, &m_histogramAlternateEmptyViewController, &m_histogramController),
  m_histogramStackViewController(&m_tabViewController, &m_histogramHeader),
  m_storeController(&m_storeHeader, snapshot->store(), &m_storeHeader),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, snapshot, &m_storeStackViewController, &m_histogramStackViewController, &m_boxHeader, &m_calculationHeader)
{
}

}
