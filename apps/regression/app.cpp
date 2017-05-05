#include "app.h"
#include "regression_icon.h"
#include "../i18n.h"

using namespace Shared;

namespace Regression {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, I18n::Message::RegressionApp, I18n::Message::RegressionAppCapital, ImageStore::RegressionIcon),
  m_store(),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController)),
  m_calculationHeader(ButtonRowController(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController)),
  m_graphController(GraphController(&m_graphAlternateEmptyViewController, &m_graphHeader, &m_store)),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController)),
  m_graphHeader(ButtonRowController(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphHeader)),
  m_storeController(&m_storeHeader, &m_store, &m_storeHeader),
  m_storeHeader(ButtonRowController(&m_storeStackViewController, &m_storeController, &m_storeController)),
  m_storeStackViewController(StackViewController(&m_tabViewController, &m_storeHeader)),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader)
{
}

}
