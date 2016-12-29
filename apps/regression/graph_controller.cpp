#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(SolidColorView(KDColorGreen)),
  m_windowButton(this, "Axes", Invocation([](void * context, void * sender) {}, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {}, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {}, this))
{
}

const char * GraphController::title() const {
  return "Graph";
}

View * GraphController::view() {
  return &m_view;
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  return false;
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
  return false;
}

const char * GraphController::emptyMessage() {
  return "Aucune donnée à tracer";
}

Responder * GraphController::defaultController() {
  return tabController();
}

Responder * GraphController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

}

