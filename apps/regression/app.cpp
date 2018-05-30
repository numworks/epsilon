#include "app.h"
#include "regression_icon.h"
#include "../i18n.h"

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
  m_rangeVersion(0)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_store.deleteAllPairs();
  m_store.setDefault();
  m_modelVersion = 0;
  m_rangeVersion = 0;
  setActiveTab(0);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

Store * App::Snapshot::store() {
  return &m_store;
}

CurveViewCursor * App::Snapshot::cursor() {
  return &m_cursor;
}

int * App::Snapshot::graphSelectedDotIndex() {
  return &m_graphSelectedDotIndex;
}

uint32_t * App::Snapshot::modelVersion() {
  return &m_modelVersion;
}

uint32_t * App::Snapshot::rangeVersion() {
  return &m_rangeVersion;
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_tabViewController),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, snapshot->store()),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_graphController(&m_graphAlternateEmptyViewController, &m_graphHeader, snapshot->store(), snapshot->cursor(), snapshot->modelVersion(), snapshot->rangeVersion(), snapshot->graphSelectedDotIndex()),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_storeController(&m_storeHeader, snapshot->store(), &m_storeHeader),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, snapshot, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader)
{
}


}
