#include "graph_app.h"
#include "graph_icon.h"

GraphApp::GraphApp() :
  App("Graph", ImageStore::GraphIcon),
  m_functionStore(Graph::FunctionStore()),
  m_listController(ListController(nullptr, &m_functionStore)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listController)),
  m_graphController(GraphController(nullptr, &m_functionStore)),
  m_valuesController(ValuesController(nullptr, &m_functionStore)),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesController)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphController, &m_valuesStackViewController),
  m_inputViewController(this, &m_tabViewController)
{
  Graph::Function * function = m_functionStore.addEmptyFunction();
  function->setContent("(x-1)*(x+1)*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("x*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("3");
  function = m_functionStore.addEmptyFunction();
  function->setContent("x*x*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("1/(1+1/x)");
  function = m_functionStore.addEmptyFunction();
  function->setContent("1/(1+1/(1/x))");
}

ViewController * GraphApp::rootViewController() {
  return &m_inputViewController;
}

InputViewController * GraphApp::inputViewController() {
  return &m_inputViewController;
}
