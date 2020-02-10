#include "histogram_parameter_controller.h"
#include "app.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

HistogramParameterController::HistogramParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store) :
  FloatParameterController<double>(parentResponder),
  m_cells{},
  m_store(store)
{
  for (int i = 0; i < k_numberOfCells; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

const char * HistogramParameterController::title() {
  return I18n::translate(I18n::Message::HistogramSet);
}

void HistogramParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  I18n::Message labels[k_numberOfCells] = {I18n::Message::RectangleWidth, I18n::Message::BarStart};
  myCell->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

double HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  return index == 0 ? m_store->barWidth() : m_store->firstDrawnBarAbscissa();
}

bool HistogramParameterController::setParameterAtIndex(int parameterIndex, double value) {
  assert(parameterIndex == 0 || parameterIndex == 1);
  const bool setBarWidth = parameterIndex == 0;

  if (setBarWidth && value <= 0.0) {
    // The bar width cannot be negative
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }

  const double nextFirstDrawnBarAbscissa = setBarWidth ? m_store->firstDrawnBarAbscissa() : value;
  const double nextBarWidth = setBarWidth ? value : m_store->barWidth();

  {
    // There should be at least one value in the drawn bin
    bool foundOneDrawnValue = false;
    for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
      if (nextFirstDrawnBarAbscissa <= m_store->maxValue(i) + nextBarWidth) {
        foundOneDrawnValue = true;
        break;
      }
    }
    if (!foundOneDrawnValue) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
  }

  // The number of bars cannot be above the max
  assert(DoublePairStore::k_numberOfSeries > 0);
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    const double min = setBarWidth ? m_store->minValue(i) : nextFirstDrawnBarAbscissa;
    double numberOfBars = std::ceil((m_store->maxValue(i) - min)/nextBarWidth);
    if (numberOfBars > Store::k_maxNumberOfBars) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
  }

  if (setBarWidth) {
    // Set the bar width
    m_store->setBarWidth(value);
  } else {
    m_store->setFirstDrawnBarAbscissa(value);
  }
  return true;
}

HighlightCell * HistogramParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < k_numberOfCells);
  return &m_cells[index];
}

}

