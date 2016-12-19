#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include "function_store.h"
#include "local_context.h"
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
  LocalContext m_localContext;
  ListController m_listController;
  HeaderViewController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  HeaderViewController m_graphHeader;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  HeaderViewController m_valuesHeader;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
