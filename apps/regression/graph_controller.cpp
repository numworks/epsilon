#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(GraphView(data)),
  m_windowParameterController(WindowParameterController(this, data)),
  m_windowButton(this, "Axes", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->windowParameterController());
  }, this)),
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
  if (!m_view.isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      headerViewController()->setSelectedButton(-1);
      m_view.selectMainView(true);
      m_view.reloadSelection();
      return true;
    }
    if (event == Ion::Events::Up) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    m_view.reloadSelection();
    int didMoveCursor = m_data->moveCursorVertically(direction);
    if ( event == Ion::Events::Up && didMoveCursor < 0) {
      m_view.selectMainView(false);
      headerViewController()->setSelectedButton(0);
      return true;
    }
    if (didMoveCursor == 0) {
      m_view.reloadSelection();
    } else {
      m_view.reload();
    }
    return (didMoveCursor >= 0);
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    m_view.reloadSelection();
    int didMoveCursor = m_data->moveCursorHorizontally(direction);
    if (didMoveCursor == 0) {
      m_view.reloadSelection();
    } else {
      m_view.reload();
    }
    return (didMoveCursor >= 0);
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

ViewController * GraphController::windowParameterController() {
  return &m_windowParameterController;
}

Responder * GraphController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * GraphController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

}

