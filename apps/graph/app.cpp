#include "app.h"
#include "graph_icon.h"

namespace Graph {

App::App(Container * container, Context * context) :
  TextFieldDelegateApp(container, &m_inputViewController, "Graph", ImageStore::GraphIcon),
  m_functionStore(FunctionStore()),
  m_xContext(XContext(context)),
  m_listController(ListController(&m_listHeader, &m_functionStore, &m_listHeader)),
  m_listHeader(HeaderViewController(nullptr, &m_listController, &m_listController)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listHeader)),
  m_graphController(GraphController(&m_graphAlternateEmptyViewController, &m_functionStore, &m_graphHeader)),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController)),
  m_graphHeader(HeaderViewController(nullptr, &m_graphAlternateEmptyViewController, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphHeader)),
  m_valuesController(&m_valuesAlternateEmptyViewController, &m_functionStore, &m_valuesHeader),
  m_valuesAlternateEmptyViewController(AlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController)),
  m_valuesHeader(HeaderViewController(nullptr, &m_valuesAlternateEmptyViewController, &m_valuesController)),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesHeader)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  return &m_xContext;
}

}
