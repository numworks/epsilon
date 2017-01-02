#include "histogram_parameter_controller.h"
#include "app.h"
#include <assert.h>

namespace Statistics {

HistogramParameterController::HistogramParameterController(Responder * parentResponder, Data * data) :
  FloatParameterController(parentResponder),
  m_binWidthCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char*)"Largeur des rectanges : ")),
  m_minValueCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char*)"Debut de la serie : ")),
  m_data(data)
{
}

const char * HistogramParameterController::title() const {
  return "Histogramme";
}

float HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_data->barWidth();
  }
  return m_data->firsBarAbscissa();
}

void HistogramParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex >= 0 && parameterIndex < 2);
  if (parameterIndex == 0) {
    if (f <= 0.0f || isnan(f) || isinf(f)) {
      app()->displayWarning("Value interdite");
      return;
    }
    m_data->setBarWidth(f);
  } else {
    m_data->setFirsBarAbscissa(f);
  }
}

int HistogramParameterController::numberOfRows() {
  return 2;
};

TableViewCell * HistogramParameterController::reusableCell(int index) {
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
