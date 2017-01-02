#include "graph_controller.h"
#include "../app.h"
#include <assert.h>
#include <math.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_view(GraphView(functionStore, &m_graphWindow)),
  m_graphWindow(functionStore),
  m_windowParameterController(WindowParameterController(this, &m_graphWindow)),
  m_zoomParameterController(ZoomParameterController(this, &m_graphWindow, &m_view)),
  m_initialisationParameterController(InitialisationParameterController(this, &m_graphWindow)),
  m_curveParameterController(CurveParameterController(&m_view, &m_graphWindow)),
  m_windowButton(this, "Axes", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->windowParameterController());
  }, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->zoomParameterController());
  }, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {
    GraphController * graphController = (GraphController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->initialisationParameterController());
  }, this)),
  m_functionStore(functionStore)
{
}

View * GraphController::view() {
  if (isnan(m_graphWindow.xCursorPosition()) && !isEmpty()) {
    m_graphWindow.initCursorPosition();
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

ViewController * GraphController::windowParameterController() {
  return &m_windowParameterController;
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
      return &m_windowButton;
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
    m_view.setContext(graphApp->localContext());
  }
  if (m_graphWindow.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_graphWindow.setContext(graphApp->localContext());
  }
  // if new functions were added to the store, the window parameters need to be refresh
  if (m_graphWindow.computeYaxis()) {
    m_graphWindow.initCursorPosition();
  }
  headerViewController()->setSelectedButton(-1);
  m_view.selectMainView(true);
  // Layout view whe the graph view that might have been modified by the zoom page
  headerViewController()->layoutView();
  // Reload graph view
  m_view.reload();
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (!m_view.isMainViewSelected()) {
    if (event == Ion::Events::Down) {
        headerViewController()->setSelectedButton(-1);
        app()->setFirstResponder(this);
        m_view.selectMainView(true);
        return true;
    }
    if (event == Ion::Events::Up) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
    }
    return headerViewController()->handleEvent(event);
  } else {
    if (event == Ion::Events::Plus) {
      m_graphWindow.zoom(1.0f/3.0f);
      m_view.reload();
      return true;
    }
    if (event == Ion::Events::Minus) {
      m_graphWindow.zoom(3.0f/4.0f);
      m_view.reload();
      return true;
    }
    if (event == Ion::Events::Right || event == Ion::Events::Left) {
      int direction = event == Ion::Events::Left ? -1 : 1;
      m_view.reloadSelection();
      if (m_graphWindow.moveCursorHorizontally(direction)) {
        m_view.reload();
      } else {
        m_view.reloadSelection();
      }
      return true;
    }
    if (event == Ion::Events::Up || event == Ion::Events::Down) {
      int direction = event == Ion::Events::Down ? -1 : 1;
      int result = m_graphWindow.moveCursorVertically(direction);
      if (result < 0) {
        if (event == Ion::Events::Down) {
          return false;
        }
        m_view.selectMainView(false);
        headerViewController()->setSelectedButton(0);
      }
      if (result == 0) {
        m_view.reloadSelection();
      }
      if (result == 1) {
        m_view.reload();
      }
      return true;
    }
    if (event == Ion::Events::OK) {
      Function * f = m_graphWindow.functionSelectedByCursor();
      m_curveParameterController.setFunction(f);
      StackViewController * stack = stackController();
      stack->push(&m_curveParameterController);
    }
    return false;
  }
}

}
