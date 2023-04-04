#include "frequency_controller.h"

#include <assert.h>
#include <escher/container.h>
#include <poincare/ieee754.h>
#include <poincare/print.h>

#include "../app.h"

namespace Statistics {

FrequencyController::FrequencyController(
    Escher::Responder *parentResponder, Escher::ButtonRowController *header,
    Escher::TabViewController *tabController,
    Escher::StackViewController *stackViewController,
    Escher::ViewController *typeViewController, Store *store)
    : PlotController(parentResponder, header, tabController,
                     stackViewController, typeViewController, store),
      m_bannerViewWithEditableField(this, App::app(), this) {
  m_view.setBannerView(&m_bannerViewWithEditableField);
  m_curveView.setCursorView(&m_cursorView);
}

void FrequencyController::didBecomeFirstResponder() {
  if (m_curveView.hasFocus()) {
    Escher::Container::activeApp()->setFirstResponder(
        m_bannerViewWithEditableField.value());
  } else {
    PlotController::didBecomeFirstResponder();
  }
}

bool FrequencyController::textFieldDidFinishEditing(
    Escher::AbstractTextField *textField, const char *text,
    Ion::Events::Event event) {
  double newX = textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  if (textFieldDelegateApp()->hasUndefinedValue(newX)) {
    return false;
  }
  // Check if x is out of bounds
  int n = totalValues(selectedSeries());
  if (newX < valueAtIndex(selectedSeries(), 0) ||
      newX > valueAtIndex(selectedSeries(), n - 1)) {
    textFieldDelegateApp()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  // Compute the new selected index
  setSelectedIndex(n - 1);
  for (int i = 0; i < n - 1; i++) {
    if (valueAtIndex(selectedSeries(), i + 1) > newX) {
      setSelectedIndex(i);
      break;
    }
  }
  // Update cursor
  m_cursor.moveTo(newX, newX, yValueAtAbscissa(selectedSeries(), newX));
  m_cursorView.setIsRing(newX ==
                         valueAtIndex(selectedSeries(), selectedIndex()));
  reloadBannerView();
  m_curveView.reload();
  return true;
}

void FrequencyController::appendLabelSuffix(Shared::AbstractPlotView::Axis axis,
                                            char *labelBuffer, int maxSize,
                                            int glyphLength,
                                            int maxGlyphLength) const {
  if (axis == Shared::AbstractPlotView::Axis::Horizontal) {
    return;
  }
  int length = strlen(labelBuffer);
  if (glyphLength >= maxGlyphLength || length + 1 >= maxSize) {
    // Suffix must fit, empty the label.
    labelBuffer[0] = 0;
    return;
  }
  assert(labelBuffer[length - 1] != '%');
  labelBuffer[length] = '%';
  labelBuffer[length + 1] = 0;
}

void FrequencyController::reloadValueInBanner(
    Poincare::Preferences::PrintFloatMode displayMode, int precision) {
  constexpr static int k_bufferSize =
      Shared::BannerView::k_maxLengthDisplayed + 1;
  char buffer[k_bufferSize] = "";
  Poincare::Print::CustomPrintf(
      buffer, k_bufferSize, "%s%s",
      I18n::translate(I18n::Message::StatisticsValue),
      I18n::translate(I18n::Message::ColonConvention));
  m_bannerViewWithEditableField.valueLabel()->setText(buffer);
  buffer[0] = 0;
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "%*.*ed", m_cursor.x(),
                                displayMode, precision);
  m_bannerViewWithEditableField.value()->setText(buffer);
}

void FrequencyController::moveCursorToSelectedIndex() {
  m_cursorView.setIsRing(true);
  m_cursorView.setColor(
      Shared::DoublePairStore::colorOfSeriesAtIndex(selectedSeries()));
  PlotController::moveCursorToSelectedIndex();
}

bool FrequencyController::moveSelectionHorizontally(
    OMG::HorizontalDirection direction) {
  assert(selectedSeries() >= 0);
  int totValues = totalValues(selectedSeries());
  if (totValues <= 1) {
    return false;
  }

  // Compute cursor step
  double step = (direction.isRight() ? 1 : -1) *
                static_cast<double>(m_graphRange.xGridUnit()) /
                static_cast<double>(k_numberOfCursorStepsInGradUnit);
  assert(step != 0.0);

  double x = m_cursor.x();

  /* Virtually increase the step so that if the cursor were to move very close
   * to a significant value, it will instead snap on this value. */
  double snapFactor = 1.5;

  assert(x >= valueAtIndex(selectedSeries(), selectedIndex()));
  int index =
      (step > 0.0 || x > valueAtIndex(selectedSeries(), selectedIndex()))
          ? selectedIndex()
          : selectedIndex() - 1;
  if (index < 0 || (step > 0.0 && index + 1 >= totValues)) {
    // Cursor cannot move further
    return false;
  }
  double xTarget = x + step * snapFactor;
  double xIndex = valueAtIndex(selectedSeries(), index);
  double xNextIndex = valueAtIndex(selectedSeries(), index + 1);
  if (xTarget <= xIndex || xTarget >= xNextIndex) {
    assert((xTarget <= xIndex) == (step < 0.0));
    assert((xTarget >= xNextIndex) == (step > 0.0));
    // Step is too big, snap on the next interesting value
    setSelectedIndex(index + (step > 0.0));
    moveCursorToSelectedIndex();
    return true;
  }
  m_cursorView.setIsRing(false);

  // Apply step
  x += step;

  // Update the newly selected index
  setSelectedIndex(index);

  // Simplify cursor's position
  double simplifyFactor = 0.5;
  // Simplification should not cross over intersting values
  assert(snapFactor >= 1 + simplifyFactor);
  if (std::fabs(x) <= std::fabs(step * simplifyFactor)) {
    x = 0.0;
  } else {
    /* Using pixelWidth(), simplify the cursor's position value while staying at
     * the same pixel. */
    double magnitude = std::pow(10.0, Poincare::IEEE754<double>::exponentBase10(
                                          m_curveView.pixelWidth()) -
                                          1.0);
    double xSimplified = magnitude * std::round(x / magnitude);
    assert(std::fabs(xSimplified - x) <= std::fabs(step * simplifyFactor));
    x = xSimplified;
  }

  // Compute the cursor's position
  double y =
      yValueForComputedXValues(selectedSeries(), index, x, xIndex, xNextIndex);
  m_cursorView.setColor(
      Shared::DoublePairStore::colorOfSeriesAtIndex(selectedSeries()));
  m_cursor.moveTo(x, x, y);
  m_curveView.reload();
  return true;
}

double FrequencyController::yValueForComputedXValues(int series, int index,
                                                     double x, double xIndex,
                                                     double xNextIndex) const {
  assert(x >= xIndex && x < xNextIndex);
  double yIndex = resultAtIndex(series, index);
  double yNextIndex = resultAtIndex(series, index + 1);
  return yIndex +
         (yNextIndex - yIndex) * ((x - xIndex) / (xNextIndex - xIndex));
}

double FrequencyController::yValueAtAbscissa(int series, double x) const {
  int n = totalValues(series);
  double currentValue = valueAtIndex(series, 0);
  if (x < currentValue) {
    return resultAtIndex(series, 0);
  }
  for (int i = 0; i < n - 1; i++) {
    double nextValue = valueAtIndex(series, i + 1);
    if (x >= currentValue && x < nextValue) {
      return yValueForComputedXValues(series, i, x, currentValue, nextValue);
    }
    currentValue = nextValue;
  }
  return resultAtIndex(series, n - 1);
}

int FrequencyController::nextSubviewWhenMovingVertically(
    OMG::VerticalDirection direction) const {
  // Search first curve in direction
  int step = direction.isUp() ? -1 : 1;
  double closestYUpOrDown = NAN;
  int nextSubview = direction.isUp() ? -1 : Store::k_numberOfSeries;
  double cursorY = m_cursor.y();
  double cursorX = m_cursor.x();
  for (int s = 1; s < Store::k_numberOfSeries; s++) {
    /* Browse series in order starting with m_selectingSeries +/- 1
     * This is useful when series are all on the same spot to properly
     * loop. */
    int seriesIndex = (selectedSeries() + step * s + Store::k_numberOfSeries) %
                      Store::k_numberOfSeries;
    assert(seriesIndex != selectedSeries());
    if (!seriesIsActive(seriesIndex)) {
      continue;
    }
    double y = yValueAtAbscissa(seriesIndex, cursorX);
    if (y == cursorY && seriesIndex * step > selectedSeries() * step) {
      // series is on the same spot and in the right direction in series list
      return seriesIndex;
    }
    if (y * step < cursorY * step &&
        (std::isnan(closestYUpOrDown) || closestYUpOrDown * step < y * step)) {
      // series is in the right direction and closer than others
      nextSubview = m_store->activeSeriesIndexFromSeriesIndex(
          seriesIndex, activeSeriesMethod());
      closestYUpOrDown = y;
    }
  }
  return nextSubview;
}

void FrequencyController::updateHorizontalIndexAfterSelectingNewSeries(
    int previousSelectedSeries) {
  // Search closest index to cursor
  double currentXValue = m_cursor.x();
  if (valueAtIndex(selectedSeries(), 0) >= currentXValue) {
    setSelectedIndex(0);
    return;
  }
  int nValues = totalValues(selectedSeries());
  double previousValue = NAN;
  for (int i = nValues - 1; i >= 0; i--) {
    double valueAtI = valueAtIndex(selectedSeries(), i);
    if (valueAtI <= currentXValue) {
      // +1 if the next index is closer than the current.
      assert(std::isnan(previousValue) || currentXValue < previousValue);
      setSelectedIndex(i + static_cast<int>(!std::isnan(previousValue) &&
                                            (currentXValue - valueAtI >
                                             previousValue - currentXValue)));
      return;
    }
    previousValue = valueAtI;
  }
  assert(false);
}

void FrequencyController::computeYBounds(float *yMin, float *yMax) const {
  // Frequency curve is always bounded between 0 and 100
  *yMin = 0.0f;
  *yMax = 100.0f;
}

}  // namespace Statistics
