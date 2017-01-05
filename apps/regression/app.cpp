#include "app.h"
#include "regression_icon.h"

namespace Regression {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, "Resgression", ImageStore::RegressionIcon),
  m_store(),
  m_calculationController(CalculationController(&m_calculationAlternateEmptyViewController, &m_store)),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(&m_tabViewController, &m_calculationController, &m_calculationController)),
  m_graphController(GraphController(&m_graphHeader, &m_graphHeader, &m_store)),
  m_graphHeader(HeaderViewController(&m_graphAlternateEmptyViewController, &m_graphController, &m_graphController)),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_graphHeader, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphAlternateEmptyViewController)),
  m_storeController(StoreController(nullptr, &m_store)),
  m_storeStackViewController(StackViewController(&m_tabViewController, &m_storeController)),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_graphStackViewController, &m_calculationAlternateEmptyViewController)
{
}

}
