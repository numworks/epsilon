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
  assert(parameterIndex >= 0 && parameterIndex < k_numberOfCells);
  if (parameterIndex == 0) {
    // Bar width

    // The bar width cannot be negative
    if (value <= 0.0) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }

    // There should be at least one value in the drawn bin
    for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
      if (m_store->firstDrawnBarAbscissa() <= m_store->maxValue(i)+value) {
        break;
      } else if (i == DoublePairStore::k_numberOfSeries - 1) {
        Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
        return false;
      }
    }

    // The number of bars cannot be above the max
    assert(DoublePairStore::k_numberOfSeries > 0);
    double maxNewNumberOfBars = std::ceil((m_store->maxValue(0) - m_store->minValue(0))/value);
    for (int i = 1; i < DoublePairStore::k_numberOfSeries; i++) {
      double numberOfBars = std::ceil((m_store->maxValue(i) - m_store->minValue(i))/value);
      if (maxNewNumberOfBars < numberOfBars) {
        maxNewNumberOfBars = numberOfBars;
      }
    }
    if (maxNewNumberOfBars > Store::k_maxNumberOfBars) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }

    // Set the bar width
    m_store->setBarWidth(value);
    return true;
  }
   assert(parameterIndex == 1);
   // The number of bars cannot be above the max
   assert(DoublePairStore::k_numberOfSeries > 0);
   const double barWidth = m_store->barWidth();
   double maxNewNumberOfBars = std::ceil((m_store->maxValue(0) - value)/barWidth);
   for (int i = 1; i < DoublePairStore::k_numberOfSeries; i++) {
     double numberOfBars = std::ceil((m_store->maxValue(i) - value)/barWidth);
     if (maxNewNumberOfBars < numberOfBars) {
       maxNewNumberOfBars = numberOfBars;
     }
   }
   if (maxNewNumberOfBars > Store::k_maxNumberOfBars) {
     Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
     return false;
   }
   // There should be at least one value in the drawn bin
   for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
     if (value <= m_store->maxValue(i) + barWidth) {
       break;
     } else if (i == DoublePairStore::k_numberOfSeries - 1) {
       Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
       return false;
     }
   }
   // Set the first drawn bar abscissa
   m_store->setFirstDrawnBarAbscissa(value);
   return true;
}

HighlightCell * HistogramParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < k_numberOfCells);
  return &m_cells[index];
}

}

