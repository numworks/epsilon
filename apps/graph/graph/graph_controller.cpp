#include "graph_controller.h"
#include "../app.h"
#include <assert.h>
#include <math.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_view(GraphView(functionStore, &m_axisInterval)),
  m_axisInterval(functionStore),
  m_axisParameterController(AxisParameterController(this, &m_axisInterval)),
  m_zoomParameterController(ZoomParameterController(this, &m_axisInterval, &m_view)),
  m_initialisationParameterController(InitialisationParameterController(this, &m_axisInterval)),
  m_axisButton(this, "Axes", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = ((StackViewController *)graphController->stackController());
    stack->push(graphController->axisParameterController());
  }, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = ((StackViewController *)graphController->stackController());
    stack->push(graphController->zoomParameterController());
  }, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = ((StackViewController *)graphController->stackController());
    stack->push(graphController->initialisationParameterController());
  }, this)),
  m_functionStore(functionStore)
{
}

View * GraphController::view() {
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

ViewController * GraphController::zoomParameterController() {
  return &m_zoomParameterController;
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
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
  if (m_view.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_view.setContext(graphApp->evaluateContext());
  }
  if (m_axisInterval.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_axisInterval.setContext(graphApp->evaluateContext());
  }
  // if new functions were added to the store, the axis interval needs to be refresh
  m_axisInterval.computeYaxes();
  headerViewController()->setSelectedButton(-1);
  m_headerSelected = false;
  // Layout view whe the graph view that might have been modified by the zoom page
  headerViewController()->layoutView();
  // Reload graph view
  m_view.reload();
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
    float xMin = m_axisInterval.xMin();
    float xMax = m_axisInterval.xMax();
    float yMin = m_axisInterval.yMin();
    float yMax = m_axisInterval.yMax();
    if (event == Ion::Events::Plus) {
      m_axisInterval.setXMin((xMax+xMin)/2.0f - fabsf(xMax-xMin)/3.0f);
      m_axisInterval.setXMax((xMax+xMin)/2.0f + fabsf(xMax-xMin)/3.0f);
      m_axisInterval.setYAuto(false);
      m_axisInterval.setYMin((yMax+yMin)/2.0f - fabsf(yMax-yMin)/3.0f);
      m_axisInterval.setYMax((yMax+yMin)/2.0f + fabsf(yMax-yMin)/3.0f);
      m_view.reload();
      return true;
    }
    if (event == Ion::Events::Minus) {
      m_axisInterval.setXMin((xMax+xMin)/2.0f - 3.0f*fabsf(xMax-xMin)/4.0f);
      m_axisInterval.setXMax((xMax+xMin)/2.0f + 3.0f*fabsf(xMax-xMin)/4.0f);
      m_axisInterval.setYAuto(false);
      m_axisInterval.setYMin((yMax+yMin)/2.0f - 3.0f*fabsf(yMax-yMin)/4.0f);
      m_axisInterval.setYMax((yMax+yMin)/2.0f + 3.0f*fabsf(yMax-yMin)/4.0f);
      m_view.reload();
      return true;
    }
    if (event == Ion::Events::Left) {
      m_view.moveCursorHorizontally(-2);
      return true;
    }
    if (event == Ion::Events::Right) {
      m_view.moveCursorHorizontally(2);
      return true;
    }
    if (event == Ion::Events::Up) {
      Function * f = m_view.moveCursorUp();
      if (f == nullptr) {
        headerViewController()->setSelectedButton(0);
        m_headerSelected = true;
      }
      return true;
    }
    if (event == Ion::Events::Down) {
      Function * f = m_view.moveCursorDown();
      if (f == nullptr) {
        return false;
      }
      return true;
    }
    return false;
  }
}

}
