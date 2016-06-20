#include "graph_app.h"

GraphApp::GraphApp() :
  App(),
  m_graphViewController(GraphController(KDColorWhite)),
  m_listViewController(ListController()),
  m_tabViewController(&m_graphViewController, &m_listViewController)
{
}

ViewController * GraphApp::rootViewController() {
  return &m_tabViewController;
}
