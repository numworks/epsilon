#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include "function_store.h"
#include "evaluate_context.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../expression_text_field_delegate.h"

namespace Graph {

class App : public ::App, public ExpressionTextFieldDelegate {
public:
  App(Container * container, Context * context);
  InputViewController * inputViewController();
  Context * evaluateContext() override;
private:
  FunctionStore m_functionStore;
  EvaluateContext m_evaluateContext;
  ListController m_listController;
  HeaderViewController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  HeaderViewController m_graphHeader;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ValuesController m_valuesController;
  HeaderViewController m_valuesHeader;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
