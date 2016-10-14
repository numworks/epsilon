#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include "function_store.h"
#include "evaluate_context.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"

namespace Graph {

class App : public ::App {
public:
  App(::Context * context);
  InputViewController * inputViewController();
  Context * globalContext();
protected:
  ViewController * rootViewController() override;
private:
  FunctionStore m_functionStore;
  Context * m_globalContext;
  EvaluateContext m_evaluateContext;
  ListController m_listController;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  ValuesController m_valuesController;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
