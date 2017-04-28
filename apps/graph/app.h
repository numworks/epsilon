#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include <poincare.h>
#include "cartesian_function_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"

namespace Graph {

class App : public Shared::FunctionApp {
public:
  App(Container * container, Poincare::Context * context);
  InputViewController * inputViewController() override;
  /* This local context can parse x. However, it always stores NAN
   * as x value. When we need to evaluate expression with a specific x value, we
   * use a temporary local context (on the stack). That way, we avoid keeping
   * weird x values after drawing curves or displaying the value table. */
  Poincare::Context * localContext() override;
private:
  CartesianFunctionStore m_functionStore;
  Poincare::VariableContext m_xContext;
  ListController m_listController;
  ButtonRowController m_listFooter;
  ButtonRowController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  ButtonRowController m_valuesHeader;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
