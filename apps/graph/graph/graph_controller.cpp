#include "graph_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_view(GraphView(functionStore)),
  m_axisInterval(functionStore),
  m_axisParameterController(AxisParameterController(this, &m_axisInterval)),
  m_axisButton(this, "Axes", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = ((StackViewController *)graphController->stackController());
    stack->push(graphController->axisParameterController());
  }, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {}, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {}, this)),
  m_functionStore(functionStore)
{
}

View * GraphController::view() {
  if (m_view.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_view.setContext(graphApp->evaluateContext());
    m_axisInterval.setContext(graphApp->evaluateContext());
  }
  return &m_view;
}

const char * GraphController::title() const {
  return "Graphique";
}

bool GraphController::isEmpty() {
  if (m_functionStore->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

const char * GraphController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return "Aucune fonction";
  }
  return "Aucune fonction selectionnee";
}

Responder * GraphController::defaultController() {
  return tabController();
}

Responder * GraphController::tabController() const{
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * GraphController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}


ViewController * GraphController::axisParameterController() {
  return &m_axisParameterController;
}

int GraphController::numberOfButtons() const {
  return 3;
}

Button * GraphController::buttonAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_axisButton;
    case 1:
      return &m_zoomButton;
    case 2:
      return &m_defaultInitialisationButton;
    default:
      assert(false);
  }
  return nullptr;
}

void GraphController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
  m_headerSelected = false;
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (m_headerSelected) {
    if (event == Ion::Events::Down) {
        headerViewController()->setSelectedButton(-1);
        m_headerSelected = false;
        return true;
    }
    if (event == Ion::Events::Up) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
    }
    return headerViewController()->handleEvent(event);
  } else {
    if (event == Ion::Events::OK) {
      m_view.moveCursorRight();
      return true;
    }
    if (event == Ion::Events::Up) {
      headerViewController()->setSelectedButton(0);
      m_headerSelected = true;
      return true;
    }
    return false;
  }
}

}
