#include "graph_app.h"

GraphApp::GraphApp() :
  App(),
  m_functionStore(Graph::FunctionStore()),
  m_listController(ListController(nullptr, &m_functionStore)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listController)),
  m_graphController(GraphController(nullptr, &m_functionStore)),
  m_tabViewController(this, &m_listStackViewController, &m_graphController)
{
  Graph::Function * function = m_functionStore.addEmptyFunction();
  function->setContent("(x-1)*(x+1)*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("x*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("3");
  function = m_functionStore.addEmptyFunction();
  function->setContent("x*x*x");
}

ViewController * GraphApp::rootViewController() {
  return &m_tabViewController;
}
