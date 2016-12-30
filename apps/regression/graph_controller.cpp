#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(GraphView(data)),
  m_windowButton(this, "Axes", Invocation([](void * context, void * sender) {}, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {}, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {}, this)),
  m_data(data)
{
}

const char * GraphController::title() const {
  return "Graph";
}

View * GraphController::view() {
  return &m_view;
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (!m_view.isMainViewSelected()) {
      headerViewController()->setSelectedButton(-1);
      m_view.selectMainView(true);
      m_view.reloadSelection();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (!m_view.isMainViewSelected()) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_view.selectMainView(false);
    headerViewController()->setSelectedButton(0);
    return true;
  }
  return false;
}

void GraphController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
  m_view.selectMainView(true);
  m_view.reload();
}


int GraphController::numberOfButtons() const {
  return 3;
}
Button * GraphController::buttonAtIndex(int index) {
  if (index == 0) {
    return &m_windowButton;
  }
  if (index == 1) {
    return &m_zoomButton;
  }
  return &m_defaultInitialisationButton;
}

bool GraphController::isEmpty() {
  if (m_data->numberOfPairs() == 0) {
    return true;
  }
  return false;
}

const char * GraphController::emptyMessage() {
  return "Aucune donnee a tracer";
}

Responder * GraphController::defaultController() {
  return tabController();
}

Responder * GraphController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

}

