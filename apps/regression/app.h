#ifndef REGRESSION_APP_H
#define REGRESSION_APP_H

#include <escher.h>
#include "../shared/text_field_delegate_app.h"
#include "store.h"
#include "store_controller.h"
#include "graph_controller.h"
#include "calculation_controller.h"

namespace Regression {

class App : public Shared::TextFieldDelegateApp {
public:
  App(Container * container);
  void reset() override;
private:
  Store m_store;
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  ButtonRowController m_calculationHeader;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  StoreController m_storeController;
  ButtonRowController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
