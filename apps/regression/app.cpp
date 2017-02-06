#include "app.h"
#include "regression_icon.h"

using namespace Shared;

namespace Regression {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_tabViewController, "Regression", "REGRESSION", ImageStore::RegressionIcon),
  m_store(),
  m_calculationController(CalculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store)),
  m_calculationAlternateEmptyViewController(AlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController)),
  m_calculationHeader(HeaderViewController(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController)),
  m_graphController(GraphController(&m_graphAlternateEmptyViewController, &m_graphHeader, &m_store)),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController)),
  m_graphHeader(HeaderViewController(nullptr, &m_graphAlternateEmptyViewController, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphHeader)),
  m_storeController(StoreController(&m_storeHeader, &m_store, &m_storeHeader)),
  m_storeHeader(HeaderViewController(nullptr, &m_storeController, &m_storeController)),
  m_storeStackViewController(StackViewController(&m_tabViewController, &m_storeHeader)),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader)
{
}

}
