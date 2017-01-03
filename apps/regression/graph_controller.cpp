#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  CurveViewWindowWithBannerAndCursorController(parentResponder, headerViewController, data, &m_view),
  m_view(GraphView(data)),
  m_data(data),
  m_initialisationParameterController(InitialisationParameterController(this, m_data)),
  m_predictionParameterController(PredictionParameterController(this, m_data))
{
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
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
  if (m_data->selectedDotIndex() == -1) {
    stackController()->push(&m_predictionParameterController);
    return true;
  }
  return false;
}

}
