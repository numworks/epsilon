#include "app.h"
#include "regression_icon.h"
#include "../apps_container.h"
#include <apps/i18n.h>

using namespace Shared;

namespace Regression {

I18n::Message App::Descriptor::name() {
  return I18n::Message::RegressionApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::RegressionAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::RegressionIcon;
}

App::Snapshot::Snapshot() :
  m_store(),
  m_cursor(),
  m_graphSelectedDotIndex(-1),
  m_modelVersion(0),
  m_rangeVersion(0),
  m_selectedSeriesIndex(-1)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this, static_cast<AppsContainer *>(container)->globalContext());
}

void App::Snapshot::reset() {
  m_store.reset();
  m_modelVersion = 0;
  m_rangeVersion = 0;
  setActiveTab(0);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::tidy() {
  m_store.setDelegate(nullptr);
  m_store.tidy();
}

App::App(Snapshot * snapshot, Poincare::Context * parentContext) :
  TextFieldDelegateApp(snapshot, &m_tabViewController),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, snapshot->store()),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_graphController(&m_graphAlternateEmptyViewController, this, &m_graphHeader, snapshot->store(), snapshot->cursor(), snapshot->modelVersion(), snapshot->previousModelsVersions(), snapshot->rangeVersion(), snapshot->graphSelectedDotIndex(), snapshot->selectedSeriesIndex()),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_storeController(&m_storeHeader, this, snapshot->store(), &m_storeHeader, parentContext),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, snapshot, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader),
  m_regressionController(nullptr, snapshot->store())
{
}


}
