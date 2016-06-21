#include "graph_controller.h"

GraphController::GraphController() :
  ViewController(),
  m_view(GraphView())
{
}

View * GraphController::view() {
  return &m_view;
}

const char * GraphController::title() const {
  return "Graph";
}

void GraphController::setFocused(bool focused) {
  /*
  if (focused) {
    App::runningApp()->focus(&m_view);
  }
  */
}

bool GraphController::handleEvent(ion_event_t event) {
  switch (event) {
    case ENTER:
      m_view.moveCursorRight();
      return true;
    default:
      return false;
  }
}
