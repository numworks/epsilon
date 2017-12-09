#include "app.h"
#include "../i18n.h"

using namespace Shared;

namespace Regression {

App::Descriptor App::Snapshot::s_descriptor;

void App::Snapshot::reset() {
  m_store.deleteAllPairs();
  m_store.setDefault();
  m_modelVersion = 0;
  m_rangeVersion = 0;
  setActiveTab(0);
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
