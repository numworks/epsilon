#include "histogram_parameter_controller.h"
#include "../app.h"
#include <algorithm>
#include <assert.h>
#include <cmath>

using namespace Shared;
using namespace Escher;

namespace Statistics {

HistogramParameterController::HistogramParameterController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store) :
  FloatParameterController<double>(parentResponder),
  m_store(store),
  m_confirmPopUpController(Invocation::Builder<HistogramParameterController>([](HistogramParameterController * controller, void * sender) {
    controller->stackController()->pop();
    return true;
  }, this))
{
  for (int i = 0; i < k_numberOfCells; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].setDelegates(inputEventHandlerDelegate, this);
  }
}

void HistogramParameterController::viewWillAppear() {
  // Initialize temporary parameters to the extracted value.
  m_tempBarWidth = extractParameterAtIndex(0);
  m_tempFirstDrawnBarAbscissa = extractParameterAtIndex(1);
  m_tempDrawCurve = extractParameterAtIndex(2);
  assert(authorizedParameters(m_tempBarWidth, m_tempFirstDrawnBarAbscissa, m_tempDrawCurve));
  FloatParameterController::viewWillAppear();
}

const char * HistogramParameterController::title() {
  return I18n::translate(I18n::Message::StatisticsGraphSettings);
}

void HistogramParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableTextWithMessage * myCell = static_cast<MessageTableCellWithEditableTextWithMessage *>(cell);
  I18n::Message labels[k_numberOfCells] = {I18n::Message::RectangleWidth, I18n::Message::BarStart, I18n::Message::DrawCurveOnHistogram};
  I18n::Message sublabels[k_numberOfCells] = {I18n::Message::RectangleWidthDescription, I18n::Message::BarStartDescrition, I18n::Message::DrawCurveOnHistogramDescription};
  myCell->setMessage(labels[index]);
  myCell->setSubLabelMessage(sublabels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool HistogramParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && (extractParameterAtIndex(0) != parameterAtIndex(0) || extractParameterAtIndex(1) != parameterAtIndex(1) || extractParameterAtIndex(2) != parameterAtIndex(2))) {
    // Temporary values are different, open pop-up to confirm discarding values
    m_confirmPopUpController.presentModally();
    return true;
  }
  return false;
}

double HistogramParameterController::extractParameterAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  switch (index)
  {
  case 0:
    return m_store->barWidth();
  case 1:
    return m_store->firstDrawnBarAbscissa();
  case 2:
    return m_store->drawCurveOverHistogram() ? 1.0 : 0.0;
  }
}

double HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  switch (index)
  {
  case 0:
    return m_tempBarWidth;
  case 1:
    return m_tempFirstDrawnBarAbscissa;
  case 2:
    return m_tempDrawCurve;
  }
}

bool HistogramParameterController::setParameterAtIndex(int parameterIndex, double value) {
  assert(parameterIndex == 0 || parameterIndex == 1 || parameterIndex == 2);
  const double nextBarWidth = parameterIndex == 0 ? value : m_tempBarWidth;
  const double nextFirstDrawnBarAbscissa = parameterIndex == 1 ? value: m_tempFirstDrawnBarAbscissa;
  const double nextDrawCurve = parameterIndex == 2 ? value: m_tempDrawCurve;


  if (!authorizedParameters(nextBarWidth, nextFirstDrawnBarAbscissa, nextDrawCurve)) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (parameterIndex == 0) {
    m_tempBarWidth = value;
  } else if (parameterIndex == 1) {
    m_tempFirstDrawnBarAbscissa = value;
  } else if (parameterIndex == 2) {
    m_tempDrawCurve = value;
  }
  return true;
}

HighlightCell * HistogramParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < k_numberOfCells);
  return &m_cells[index];
}

void HistogramParameterController::buttonAction() {
  // Update parameters values and proceed.
  assert(authorizedParameters(m_tempBarWidth, m_tempFirstDrawnBarAbscissa, m_tempDrawCurve));
  m_store->setBarWidth(m_tempBarWidth);
  m_store->setFirstDrawnBarAbscissa(m_tempFirstDrawnBarAbscissa);
  m_store->setDrawCurveOverHistogram(m_tempDrawCurve == 1.0);
  FloatParameterController::buttonAction();
}

bool HistogramParameterController::authorizedParameters(double barWidth, double firstDrawnBarAbscissa, double drawCurve) {
  if (barWidth < 0.0) {
    // The bar width cannot be negative
    return false;
  }

  if (drawCurve != 0.0 && drawCurve != 1.0) {
    // temporary hack: this should be a boolean
    return false;
  }

  assert(DoublePairStore::k_numberOfSeries > 0);
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (!Shared::DoublePairStore::DefaultValidSeries(m_store, i)) {
      continue;
    }
    const double min = std::min(m_store->minValue(i), firstDrawnBarAbscissa);
    const double max = m_store->maxValue(i);
    double numberOfBars = std::ceil((max - min) / barWidth);
    // First escape case: if the bars are too thin or there is too much bars
    if (numberOfBars > HistogramRange::k_maxNumberOfBars
    // Second escape case : max < X-start
        || max < firstDrawnBarAbscissa
    /* Third escape case: Since interval width is computed in float, we
     * need to check if the values are not too close.
     * If max == min then the interval goes from min to min + barWidth.
     * But if min == min + barWidth, the display is bugged. */
        || (static_cast<float>(min) == static_cast<float>(max) && static_cast<float>(min + barWidth) == static_cast<float>(min)))
    {
      return false;
    }
  }
  return true;
}

}

