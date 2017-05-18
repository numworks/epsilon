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

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_store.deleteAllPairs();
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

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_tabViewController),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, snapshot->store()),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_graphController(&m_graphAlternateEmptyViewController, &m_graphHeader, snapshot->store(), snapshot->cursor()),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_storeController(&m_storeHeader, snapshot->store(), &m_storeHeader),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader)
{
}


}
