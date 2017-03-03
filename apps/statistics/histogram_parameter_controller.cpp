#include "histogram_parameter_controller.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

HistogramParameterController::HistogramParameterController(Responder * parentResponder, Store * store) :
  FloatParameterController(parentResponder),
  m_binWidthCell(PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char*)"Largeur des rectangles : ")),
  m_minValueCell(PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char*)"Debut de la serie : ")),
  m_store(store)
{
}

const char * HistogramParameterController::title() const {
  return "Parametres de l'histogramme";
}

float HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_store->barWidth();
  }
  return m_store->firstDrawnBarAbscissa();
}

void HistogramParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex >= 0 && parameterIndex < 2);
  if (parameterIndex == 0) {
    if (f <= 0.0f || isnan(f) || isinf(f)) {
      app()->displayWarning("Valeur interdite");
      return;
    }
    m_store->setBarWidth(f);
  } else {
    m_store->setFirstDrawnBarAbscissa(f);
  }
}

int HistogramParameterController::numberOfRows() {
  return 2;
};

HighlightCell * HistogramParameterController::reusableCell(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_binWidthCell;
  }
  return &m_minValueCell;
}

int HistogramParameterController::reusableCellCount() {
  return 2;
}

}
