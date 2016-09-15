#include "graph_app.h"

GraphApp::GraphApp() :
  App(),
  m_functionStore(Graph::FunctionStore()),
  m_listController(ListController(nullptr, &m_functionStore)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listController)),
  m_graphController(GraphController(nullptr, &m_functionStore)),
  m_tabViewController(this, &m_listStackViewController, &m_graphController)
{
  m_functionStore.pushFunction("(x-1)*(x+1)*x");
  m_functionStore.pushFunction("x*x");
  m_functionStore.pushFunction("3");
  m_functionStore.pushFunction("x*x*x");
}

ViewController * GraphApp::rootViewController() {
  return &m_tabViewController;
}
