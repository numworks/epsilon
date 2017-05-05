#include "app.h"
#include "regression_icon.h"
#include "../i18n.h"

using namespace Shared;

namespace Regression {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, I18n::Message::RegressionApp, I18n::Message::RegressionAppCapital, ImageStore::RegressionIcon),
  m_store(),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_graphController(&m_graphAlternateEmptyViewController, &m_graphHeader, &m_store),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_storeController(&m_storeHeader, &m_store, &m_storeHeader),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader)
{
}

}
