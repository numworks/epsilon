#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include <poincare.h>
#include "function_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../text_field_delegate_app.h"

namespace Graph {

class App : public TextFieldDelegateApp {
public:
  App(Container * container, Context * context);
  InputViewController * inputViewController();
  Context * localContext() override;
private:
  FunctionStore m_functionStore;
  VariableContext m_xContext;
  ListController m_listController;
  HeaderViewController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  HeaderViewController m_graphHeader;
  StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  HeaderViewController m_valuesHeader;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
