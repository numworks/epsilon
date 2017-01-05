#include "graph_controller.h"

namespace Regression {

GraphController::GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store) :
  CurveViewWindowWithBannerAndCursorController(parentResponder, headerViewController, store, &m_view),
  m_view(GraphView(store, &m_bannerView, &m_cursorView)),
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

void GraphController::reloadBannerView() {
  m_bannerView.setLegendAtIndex((char *)"y = ax+b", 0);
  char buffer[k_maxNumberOfCharacters + Constant::FloatBufferSizeInScientificMode];
  const char * legend = "a = ";
  float slope = m_store->slope();
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(slope).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_bannerView.setLegendAtIndex(buffer, 1);

  legend = "b = ";
  float yIntercept = m_store->yIntercept();
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(yIntercept).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_bannerView.setLegendAtIndex(buffer, 2);

  legend = "x = ";
  float x = m_store->xCursorPosition();
  // Display a specific legend if the mean dot is selected
  if (m_store->selectedDotIndex() == m_store->numberOfPairs()) {
    legend = "x^ = ";
    x = m_store->meanOfColumn(0);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(x).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_bannerView.setLegendAtIndex(buffer, 3);

  legend = "y = ";
  float y = m_store->yCursorPosition();
  if (m_store->selectedDotIndex() == m_store->numberOfPairs()) {
    legend = "y^ = ";
    y = m_store->meanOfColumn(1);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  Float(y).convertFloatToText(buffer+legendLength, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_bannerView.setLegendAtIndex(buffer, 4);

  m_bannerView.layoutSubviews();
}

}
