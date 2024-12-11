#include "histogram_parameter_controller.h"

#include <assert.h>

#include <algorithm>
#include <cmath>

#include "../app.h"

using namespace Shared;
using namespace Escher;

namespace Statistics {

HistogramParameterController::HistogramParameterController(
    Responder* parentResponder, Store* store)
    : FloatParameterController<double>(parentResponder),
      m_store(store),
      m_confirmPopUpController(
          Invocation::Builder<HistogramParameterController>(
              [](HistogramParameterController* controller, void* sender) {
                controller->stackController()->pop();
                return true;
              },
              this)) {
  for (int i = 0; i < k_numberOfCells; i++) {
    m_cells[i].setParentResponder(&m_selectableListView);
    m_cells[i].setDelegate(this);
  }
  m_cells[0].label()->setMessage(I18n::Message::RectangleWidth);
  m_cells[0].subLabel()->setMessage(I18n::Message::RectangleWidthDescription);
  m_cells[1].label()->setMessage(I18n::Message::BarStart);
  m_cells[1].subLabel()->setMessage(I18n::Message::BarStartDescrition);
}

void HistogramParameterController::viewWillAppear() {
  // Initialize temporary parameters to the extracted value.
  m_tempBarWidth = extractParameterAtIndex(0);
  m_tempFirstDrawnBarAbscissa = extractParameterAtIndex(1);
  assert(authorizedParameters(m_tempBarWidth, m_tempFirstDrawnBarAbscissa));
  FloatParameterController::viewWillAppear();
}

const char* HistogramParameterController::title() const {
  return I18n::translate(I18n::Message::StatisticsGraphSettings);
}

KDCoordinate HistogramParameterController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    return m_cells[row].minimalSizeForOptimalDisplay().height();
  }
  assert(type == k_buttonCellType);
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(row);
}

bool HistogramParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back &&
      (extractParameterAtIndex(0) != parameterAtIndex(0) ||
       extractParameterAtIndex(1) != parameterAtIndex(1))) {
    // Temporary values are different, open pop-up to confirm discarding values
    m_confirmPopUpController.presentModally();
    return true;
  }
  return false;
}

double HistogramParameterController::extractParameterAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  return index == 0 ? m_store->barWidth() : m_store->firstDrawnBarAbscissa();
}

double HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  return index == 0 ? m_tempBarWidth : m_tempFirstDrawnBarAbscissa;
}

bool HistogramParameterController::setParameterAtIndex(int parameterIndex,
                                                       double value) {
  assert(parameterIndex == 0 || parameterIndex == 1);
  const double nextBarWidth = parameterIndex == 0 ? value : m_tempBarWidth;
  const double nextFirstDrawnBarAbscissa =
      parameterIndex == 0 ? m_tempFirstDrawnBarAbscissa : value;
  if (!authorizedParameters(nextBarWidth, nextFirstDrawnBarAbscissa)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (parameterIndex == 0) {
    m_tempBarWidth = value;
  } else {
    m_tempFirstDrawnBarAbscissa = value;
  }
  return true;
}

HighlightCell* HistogramParameterController::reusableParameterCell(int index,
                                                                   int type) {
  assert(index >= 0 && index < k_numberOfCells);
  return &m_cells[index];
}

Escher::TextField* HistogramParameterController::textFieldOfCellAtIndex(
    HighlightCell* cell, int index) {
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<
             MenuCellWithEditableText<MessageTextView, MessageTextView>*>(cell)
      ->textField();
}

void HistogramParameterController::buttonAction() {
  // Update parameters values and proceed.
  assert(authorizedParameters(m_tempBarWidth, m_tempFirstDrawnBarAbscissa));
  m_store->setBarWidth(m_tempBarWidth);
  m_store->setFirstDrawnBarAbscissa(m_tempFirstDrawnBarAbscissa);
  FloatParameterController::buttonAction();
}

bool HistogramParameterController::authorizedParameters(
    double barWidth, double firstDrawnBarAbscissa) {
  if (barWidth < 0.0) {
    // The bar width cannot be negative
    return false;
  }
  // TODO: Histogram range is in float so we can't go over k_maxFloat
  if (firstDrawnBarAbscissa > Poincare::Range1D<float>::k_maxFloat ||
      firstDrawnBarAbscissa < -Poincare::Range1D<float>::k_maxFloat ||
      barWidth > Poincare::Range1D<float>::k_maxFloat) {
    return false;
  }
  assert(DoublePairStore::k_numberOfSeries > 0);
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (Shared::DoublePairStore::DefaultActiveSeriesTest(m_store, i) &&
        m_store->maxValue(i) < firstDrawnBarAbscissa) {
      return false;
    }
  }
  return AuthorizedBarWidth(barWidth, firstDrawnBarAbscissa, m_store);
}

bool HistogramParameterController::AuthorizedBarWidth(
    double barWidth, double firstDrawnBarAbscissa, Store* store) {
  for (int i = 0; i < DoublePairStore::k_numberOfSeries; i++) {
    if (!Shared::DoublePairStore::DefaultActiveSeriesTest(store, i)) {
      continue;
    }
    const double min = std::min(store->minValue(i), firstDrawnBarAbscissa);
    const double max = store->maxValue(i);
    double numberOfBars = std::ceil((max - min) / barWidth);
    // First escape case: if the bars are too thin or there is too much bars
    if (numberOfBars > HistogramRange::k_maxNumberOfBars
        /* Second escape case: Since interval width is computed in float, we
         * need to check if the values are not too close.
         * If max == min then the interval goes from min to min + barWidth.
         * But if min == min + barWidth, the display is bugged. */
        || (static_cast<float>(min) == static_cast<float>(max) &&
            static_cast<float>(min + barWidth) == static_cast<float>(min))) {
      return false;
    }
  }
  return true;
}

}  // namespace Statistics
