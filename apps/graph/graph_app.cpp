#include "graph_app.h"

GraphApp::GraphApp() :
  App(),
  m_listController(ListController()),
  m_graphController(GraphController()),
  m_tabViewController(&m_listController, &m_graphController)
{
}

ViewController * GraphApp::rootViewController() {
  return &m_tabViewController;
}
