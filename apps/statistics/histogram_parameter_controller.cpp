#include "histogram_parameter_controller.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

HistogramParameterController::HistogramParameterController(Responder * parentResponder, Store * store) :
  FloatParameterController(parentResponder, "Valider"),
  m_cells{PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr)},
  m_store(store)
{
}

const char * HistogramParameterController::title() const {
  return "Parametres de l'histogramme";
}

void HistogramParameterController::viewWillAppear() {
  for (int i = 0; i < 2; i++) {
    m_previousParameters[i] = parameterAtIndex(i);
  }
  FloatParameterController::viewWillAppear();
}

int HistogramParameterController::numberOfRows() {
  return 1+2;
}

void HistogramParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  PointerTableCellWithEditableText * myCell = (PointerTableCellWithEditableText *)cell;
  const char * labels[2] = {"Largeur des rectangles : ", "Debut de la serie : "};
  myCell->setText(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

float HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_store->barWidth();
  }
  return m_store->firstDrawnBarAbscissa();
}

float HistogramParameterController::previousParameterAtIndex(int index) {
  return m_previousParameters[index];
}

void HistogramParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex >= 0 && parameterIndex < 2);
  if (parameterIndex == 0) {
    if (f <= 0.0f) {
      app()->displayWarning("Valeur interdite");
      return;
    }
    m_store->setBarWidth(f);
  } else {
    m_store->setFirstDrawnBarAbscissa(f);
  }
}

HighlightCell * HistogramParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < 2);
  return &m_cells[index];
}

int HistogramParameterController::reusableParameterCellCount(int type) {
  return 2;
}

}
