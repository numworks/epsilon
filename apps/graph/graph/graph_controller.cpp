#include "graph_controller.h"
#include "../app.h"
#include <assert.h>
#include <math.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  CurveViewWindowWithBannerAndCursorController(parentResponder, header, &m_graphWindow, &m_view),
  m_view(GraphView(functionStore, &m_graphWindow)),
  m_graphWindow(functionStore),
  m_initialisationParameterController(InitialisationParameterController(this, &m_graphWindow)),
  m_curveParameterController(CurveParameterController(&m_view, &m_graphWindow)),
  m_functionStore(functionStore)
{
}

View * GraphController::view() {
  if (isnan(m_graphWindow.xCursorPosition()) && !isEmpty()) {
    m_graphWindow.initCursorPosition();
  }
  return &m_view;
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

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
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
  CurveViewWindowWithBannerAndCursorController::didBecomeFirstResponder();
}

bool GraphController::handleEnter() {
  Function * f = m_graphWindow.functionSelectedByCursor();
  m_curveParameterController.setFunction(f);
  StackViewController * stack = stackController();
  stack->push(&m_curveParameterController);
  return true;
}

}
