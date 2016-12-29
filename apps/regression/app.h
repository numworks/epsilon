#ifndef REGRESSION_APP_H
#define REGRESSION_APP_H

#include <escher.h>
#include "../text_field_delegate_app.h"
#include "data_controller.h"
#include "graph_controller.h"
#include "calculation_controller.h"

namespace Regression {

class App : public TextFieldDelegateApp {
public:
  App(Container * container);
private:
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  GraphController m_graphController;
  HeaderViewController m_graphHeader;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  StackViewController m_graphStackViewController;
  DataController m_dataController;
  StackViewController m_dataStackViewController;
  TabViewController m_tabViewController;
};

}

#endif
