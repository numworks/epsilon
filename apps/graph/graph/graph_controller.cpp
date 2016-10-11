#include "graph_controller.h"

GraphController::GraphController(Responder * parentResponder, Graph::FunctionStore * functionStore) :
  HeaderViewController(parentResponder, &m_view),
  m_view(GraphView(functionStore)),
  m_headerSelected(false)
{
  setButtonTitles("Fenetre", "Affichage", nullptr);
}

const char * GraphController::title() const {
  return "Graphique";
}

Responder * GraphController::tabController() const{
  return (parentResponder());
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
