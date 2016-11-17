#include "graph_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

GraphController::GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_view(GraphView(functionStore)),
  m_windowButton(Button(this, "Fenetre", Invocation([](void * context, void * sender) {}, this))),
  m_displayButton(this, "Affichage", Invocation([](void * context, void * sender) {}, this)),
  m_functionStore(functionStore)
{
}

View * GraphController::view() {
  if (m_view.context() == nullptr) {
    App * graphApp = (Graph::App *)app();
    m_view.setContext(graphApp->evaluateContext());
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
