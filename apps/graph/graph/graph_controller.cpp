#include "graph_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore) :
  HeaderViewController(parentResponder, &m_view),
  m_view(GraphView(functionStore)),
  m_headerSelected(false),
  m_windowButton(Button(this, "Fenetre", Invocation([](void * context, void * sender) {}, this))),
  m_displayButton(this, "Affichage", Invocation([](void * context, void * sender) {}, this))
{
}

View * GraphController::view() {
  if (m_view.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_view.setContext(graphApp->evaluateContext());
  }
  return HeaderViewController::view();
}

const char * GraphController::title() const {
  return "Graphique";
}

Responder * GraphController::tabController() const{
  return (parentResponder());
}

int GraphController::numberOfButtons() const {
  return 2;
}

Button * GraphController::buttonAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_windowButton;
    case 1:
      return &m_displayButton;
    default:
      assert(false);
  }
  return nullptr;
}

void GraphController::didBecomeFirstResponder() {
  m_headerSelected = false;
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (m_headerSelected) {
    if (event == Ion::Events::Down) {
        setSelectedButton(-1);
        m_headerSelected = false;
        return true;
    }
    if (event == Ion::Events::Up) {
      setSelectedButton(-1);
      app()->setFirstResponder(tabController());
    }
    return HeaderViewController::handleEvent(event);
  } else {
    if (event == Ion::Events::OK) {
      m_view.moveCursorRight();
      return true;
    }
    if (event == Ion::Events::Up) {
      setSelectedButton(0);
      m_headerSelected = true;
      return true;
    }
    return false;
  }
}

}
