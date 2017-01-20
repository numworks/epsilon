#ifndef REGRESSION_APP_H
#define REGRESSION_APP_H

#include <escher.h>
#include "../text_field_delegate_app.h"
#include "store.h"
#include "../store_controller.h"
#include "graph_controller.h"
#include "calculation_controller.h"

namespace Regression {

class App : public TextFieldDelegateApp {
public:
  App(Container * container);
private:
  Store m_store;
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  HeaderViewController m_calculationHeader;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  HeaderViewController m_graphHeader;
  StackViewController m_graphStackViewController;
  ::StoreController m_storeController;
  HeaderViewController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
