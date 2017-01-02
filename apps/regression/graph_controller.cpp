#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  CurveViewWindowWithBannerAndCursorController(parentResponder, headerViewController, data, &m_view),
  m_view(GraphView(data)),
  m_data(data)
{
}

ViewController * GraphController::initialisationParameterController() {
  return &m_windowParameterController;
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

bool GraphController::handleEnter() {
  return false;
}

}
