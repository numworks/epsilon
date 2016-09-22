#include "graph_app.h"

GraphApp::GraphApp() :
  App(),
  m_functionStore(Graph::FunctionStore()),
  m_listController(ListController(nullptr, &m_functionStore)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listController)),
  m_graphController(GraphController(nullptr, &m_functionStore)),
  m_tabViewController(this, &m_listStackViewController, &m_graphController)
{
  Graph::Function function = Graph::Function("(x-1)*(x+1)*x",
      Graph::FunctionStore::defaultColors[(m_functionStore.numberOfFunctions())%Graph::FunctionStore::numberOfDefaultColors]);
  m_functionStore.addFunction(&function);
  function = Graph::Function("x*x",
      Graph::FunctionStore::defaultColors[(m_functionStore.numberOfFunctions())%Graph::FunctionStore::numberOfDefaultColors]);
  m_functionStore.addFunction(&function);
  function = Graph::Function("3",
      Graph::FunctionStore::defaultColors[(m_functionStore.numberOfFunctions())%Graph::FunctionStore::numberOfDefaultColors]);
  m_functionStore.addFunction(&function);
  function = Graph::Function("x*x*x",
      Graph::FunctionStore::defaultColors[(m_functionStore.numberOfFunctions())%Graph::FunctionStore::numberOfDefaultColors]);
  m_functionStore.addFunction(&function);
}

ViewController * GraphApp::rootViewController() {
  return &m_tabViewController;
}
