#include "graph_controller.h"
#include <assert.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore, EvaluateContext * evaluateContext) :
  HeaderViewController(parentResponder, &m_view),
  m_view(GraphView(functionStore, evaluateContext)),
  m_headerSelected(false),
  m_windowButton(Button(this, "Fenetre", Invocation([](void * context, void * sender) {}, this))),
  m_displayButton(this, "Affichage", Invocation([](void * context, void * sender) {}, this))
{
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
    switch (event) {
      case Ion::Events::Event::DOWN_ARROW:
        setSelectedButton(-1);
        m_headerSelected = false;
        return true;
      case Ion::Events::Event::UP_ARROW:
        setSelectedButton(-1);
        app()->setFirstResponder(tabController());
      default:
        return HeaderViewController::handleEvent(event);
    }
  } else {
    switch (event) {
      case Ion::Events::Event::ENTER:
        m_view.moveCursorRight();
        return true;
      case Ion::Events::Event::UP_ARROW:
        setSelectedButton(0);
        m_headerSelected = true;
        return true;
      default:
        return false;
    }
  }
}

}
