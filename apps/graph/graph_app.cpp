#include "graph_app.h"

GraphApp::GraphApp() :
  App(),
  m_listController(ListController(nullptr)),
  m_graphController(GraphController(nullptr)),
  m_tabViewController(this, &m_listController, &m_graphController)
{
}

ViewController * GraphApp::rootViewController() {
  return &m_tabViewController;
}
