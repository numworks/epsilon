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

ExpressionTextFieldDelegate * HistogramParameterController::textFieldDelegate() {
  ExpressionTextFieldDelegate * myApp = (ExpressionTextFieldDelegate *)app();
  return myApp;
}

const char * HistogramParameterController::title() const {
  return "Histogramme";
}

float HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_data->binWidth();
  }
  return m_data->minValue();
}

void HistogramParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex >= 0 && parameterIndex < 2);
  if (parameterIndex == 0) {
    m_data->setBinWidth(f);
  } else {
    m_data->setMinValue(f);
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
