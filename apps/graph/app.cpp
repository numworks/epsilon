#include "app.h"
#include "graph_icon.h"

namespace Graph {

App::App(Container * container, Context * context) :
  ::App(container, &m_inputViewController, "Graph", ImageStore::GraphIcon),
  ExpressionTextFieldDelegate(),
  m_functionStore(FunctionStore()),
  m_localContext(LocalContext(context)),
  m_listController(ListController(&m_listHeader, &m_functionStore, &m_listHeader)),
  m_listHeader(HeaderViewController(nullptr, &m_listController, &m_listController)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listHeader)),
  m_graphController(GraphController(&m_graphHeader, &m_functionStore, &m_graphHeader)),
  m_graphHeader(HeaderViewController(&m_graphAlternateEmptyViewController, &m_graphController, &m_graphController)),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_graphHeader, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphAlternateEmptyViewController)),
  m_valuesController(&m_valuesHeader, &m_functionStore, &m_valuesHeader),
  m_valuesHeader(HeaderViewController(&m_valuesAlternateEmptyViewController, &m_valuesController, &m_valuesController)),
  m_valuesAlternateEmptyViewController(AlternateEmptyViewController(nullptr, &m_valuesHeader, &m_valuesController)),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesAlternateEmptyViewController)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  return &m_localContext;
}

}
