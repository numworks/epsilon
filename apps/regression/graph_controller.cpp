#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store) :
  CurveViewWindowWithBannerAndCursorController(parentResponder, headerViewController, store, &m_view),
  m_view(GraphView(store)),
  m_store(store),
  m_initialisationParameterController(InitialisationParameterController(this, m_store)),
  m_predictionParameterController(PredictionParameterController(this, m_store))
{
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

void GraphController::didBecomeFirstResponder() {
  uint32_t storeChecksum = m_store->checksum();
  if (m_storeVersion != storeChecksum) {
    m_storeVersion = storeChecksum;
    m_store->initWindowParameters();
    m_store->initCursorPosition();
  }
  CurveViewWindowWithBannerAndCursorController::didBecomeFirstResponder();
}


bool GraphController::isEmpty() {
  if (m_store->numberOfPairs() < 2 || isinf(m_store->slope()) || isnan(m_store->slope())) {
    return true;
  }
  return false;
}

const char * GraphController::emptyMessage() {
  if (m_store->numberOfPairs() == 0) {
    return "Aucune donnee a tracer";
  }
  return "Pas assez de donnees pour un regression";
}

bool GraphController::handleEnter() {
  if (m_store->selectedDotIndex() == -1) {
    stackController()->push(&m_predictionParameterController);
    return true;
  }
  return false;
}

}
