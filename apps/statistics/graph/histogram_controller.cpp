#include "histogram_controller.h"

#include <apps/global_preferences.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <float.h>
#include <omg/ieee754.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include <algorithm>
#include <cmath>

#include "../app.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Statistics {

HistogramController::HistogramController(
    Responder* parentResponder, ButtonRowController* header,
    TabViewController* tabController,
    Escher::StackViewController* stackViewController,
    Escher::ViewController* typeViewController, Store* store,
    uint32_t* storeVersion)
    : MultipleDataViewController(parentResponder, tabController, header,
                                 stackViewController, typeViewController,
                                 store),
      m_view(store, &m_histogramRange),
      m_histogramRange(store),
      m_storeVersion(storeVersion),
      m_histogramParameterController(nullptr, store),
      m_parameterButton(
          this, I18n::Message::StatisticsGraphSettings,
          Invocation::Builder<HistogramController>(
              [](HistogramController* histogramController, void* sender) {
                histogramController->stackController()->push(
                    histogramController->histogramParameterController());
                return true;
              },
              this),
          KDFont::Size::Small) {}

ButtonCell* HistogramController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  return index == 0 ? GraphButtonRowDelegate::buttonAtIndex(index, position)
                    : const_cast<SimpleButtonCell*>(&m_parameterButton);
}

bool HistogramController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Toolbox) {
    stackController()->push(histogramParameterController());
    return true;
  }
  return MultipleDataViewController::handleEvent(event);
}

void HistogramController::viewWillAppearBeforeReload() {
  uint32_t storeChecksum = m_store->storeChecksum();
  if (*m_storeVersion != storeChecksum) {
    *m_storeVersion = storeChecksum;
    initBarParameters();
  }

  initRangeParameters();
  sanitizeSelectedIndex();
}

void HistogramController::highlightSelection() {
  HistogramView* selectedHistogramView =
      static_cast<HistogramView*>(m_view.plotViewForSeries(selectedSeries()));
  selectedHistogramView->setHighlight(
      m_store->startOfBarAtIndex(selectedSeries(), selectedIndex()),
      m_store->endOfBarAtIndex(selectedSeries(), selectedIndex()));
  // if the selectedBar was outside of range, we need to scroll
  if (m_histogramRange.scrollToSelectedBarIndex(selectedSeries(),
                                                selectedIndex())) {
    multipleDataView()->reload();
  }
}

bool HistogramController::reloadBannerView() {
  if (selectedSeries() < 0) {
    return false;
  }
  KDCoordinate previousHeight =
      m_view.bannerView()->minimalSizeForOptimalDisplay().height();

  int precision = Preferences::SharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode =
      Poincare::Preferences::SharedPreferences()->displayMode();
  constexpr static int k_bufferSize =
      sizeof("Intervalle : [-1.2345ᴇ-123;-1.2345ᴇ-123[");  // longest case
  constexpr static int k_maxNumberOfGlyphs =
      Poincare::Print::k_maxNumberOfSmallGlyphsInScreenWidth;
  char buffer[k_bufferSize] = "";

  // Display series name
  m_store->tableName(selectedSeries(), buffer, k_bufferSize);
  m_view.bannerView()->seriesName()->setText(buffer);

  // Display interval
  double lowerBound =
      m_store->startOfBarAtIndex(selectedSeries(), selectedIndex());
  double upperBound =
      m_store->endOfBarAtIndex(selectedSeries(), selectedIndex());
  Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
      buffer, k_bufferSize, precision, k_maxNumberOfGlyphs,
      "%s%s[%*.*ed,%*.*ed%s", I18n::translate(I18n::Message::Interval),
      I18n::translate(I18n::Message::ColonConvention), lowerBound, displayMode,
      upperBound, displayMode,
      GlobalPreferences::SharedGlobalPreferences()->openIntervalChar(false));
  m_view.bannerView()->intervalView()->setText(buffer);

  // Display frequency
  double size = m_store->heightOfBarAtIndex(selectedSeries(), selectedIndex());
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "%s%s%*.*ed",
                                I18n::translate(I18n::Message::Frequency),
                                I18n::translate(I18n::Message::ColonConvention),
                                size, displayMode, precision);
  m_view.bannerView()->frequencyView()->setText(buffer);

  // Display relative frequency
  double relativeFrequency = size / m_store->sumOfOccurrences(selectedSeries());
  Poincare::Print::CustomPrintf(
      buffer, k_bufferSize, "%s%s%*.*ed",
      I18n::translate(I18n::Message::RelativeFrequency),
      I18n::translate(I18n::Message::ColonConvention), relativeFrequency,
      displayMode, precision);
  m_view.bannerView()->relativeFrequencyView()->setText(buffer);

  m_view.bannerView()->reload();
  return previousHeight !=
         m_view.bannerView()->minimalSizeForOptimalDisplay().height();
}

bool HistogramController::moveSelectionHorizontally(
    OMG::HorizontalDirection direction) {
  int numberOfBars = m_store->numberOfBars(selectedSeries());
  int newSelectedBarIndex = selectedIndex();
  do {
    newSelectedBarIndex += direction.isRight() ? 1 : -1;
  } while (newSelectedBarIndex >= 0 && newSelectedBarIndex < numberOfBars &&
           m_store->heightOfBarAtIndex(selectedSeries(), newSelectedBarIndex) ==
               0);

  if (newSelectedBarIndex >= 0 && newSelectedBarIndex < numberOfBars &&
      selectedIndex() != newSelectedBarIndex) {
    setSelectedIndex(newSelectedBarIndex);
    highlightSelection();
    return true;
  }
  return false;
}

void HistogramController::preinitXRangeParameters(double* xMin, double* xMax) {
  /* Compute m_store's min and max values, hold them temporarily in the
   * CurveViewRange, for later use by initRangeParameters and
   * initBarParameters. Indeed, initRangeParameters will anyway alter the
   * CurveViewRange. The CurveViewRange setter methods take care of the case
   * where minValue >= maxValue. Moreover they compute the xGridUnit, which is
   * used by initBarParameters. */
  double minValue = DBL_MAX;
  double maxValue = -DBL_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (activeSeriesMethod()(m_store, i)) {
      minValue = std::min<double>(minValue, m_store->minValue(i));
      maxValue = std::max<double>(maxValue, m_store->maxValue(i));
    }
  }
  assert(xMin != nullptr);
  *xMin = minValue;
  if (xMax != nullptr) {
    *xMax = maxValue;
  }
  m_histogramRange.setHistogramRange(minValue, maxValue);
}

void HistogramController::initRangeParameters() {
  assert(activeSeriesMethod()(m_store, selectedSeries()));
  double barWidth = m_store->barWidth();
  double xStart = m_store->firstDrawnBarAbscissa();
  double minValue, maxValue;
  preinitXRangeParameters(&minValue, &maxValue);

  /* The range of bar at index barIndex is :
   * [xStart + barWidth * barIndex ; xStart + barWidth * (barIndex + 1)] */

  double barIndexMin = std::floor((minValue - xStart) / barWidth + FLT_EPSILON);
  double barIndexMax = std::floor((maxValue - xStart) / barWidth + FLT_EPSILON);

  // barIndexMax is the right end of the last bar
  barIndexMax += 1.;

  /* If a bar is represented by less than one pixel, we cap xMax */
  barIndexMax = std::min(barIndexMax, barIndexMin + k_maxNumberOfBarsPerWindow);

  double xMin = xStart + barWidth * barIndexMin;
  double xMax = xStart + barWidth * barIndexMax;

  // TODO: Set the histogram range to double.
  float min = std::clamp(static_cast<float>(xMin), -Range1D<float>::k_maxFloat,
                         Range1D<float>::k_maxFloat);
  float max = std::clamp(static_cast<float>(xMax), -Range1D<float>::k_maxFloat,
                         Range1D<float>::k_maxFloat);

  m_histogramRange.setHistogramRange(
      min - HistogramRange::k_displayLeftMarginRatio * (max - min),
      max + HistogramRange::k_displayRightMarginRatio * (max - min));

  initYRangeParameters(selectedSeries());
}

void HistogramController::initYRangeParameters(int series) {
  assert(activeSeriesMethod()(m_store, series));
  /* Height of drawn bar are relative to the maximal bar of the series, so all
   * displayed series need the same range of [0,1]. */
  float yMax = 1.0f + HistogramRange::k_displayTopMarginRatio;

  /* Compute YMin:
   *    ratioFloatPixel*(0-yMin) = bottomMargin
   *    ratioFloatPixel*(yMax-yMin) = viewHeight
   *
   *    -ratioFloatPixel*yMin = bottomMargin
   *    ratioFloatPixel*yMax-ratioFloatPixel*yMin = viewHeight
   *
   *    ratioFloatPixel = (viewHeight - bottomMargin)/yMax
   *    yMin = -bottomMargin/ratioFloatPixel = yMax*bottomMargin/(bottomMargin -
   * viewHeight)
   * */
  float bottomMargin = static_cast<float>(HistogramRange::k_bottomMargin);
  float viewHeight = static_cast<float>(m_view.subviewHeight());
  float yMin = yMax * bottomMargin / (bottomMargin - viewHeight);

  m_histogramRange.setYRange(yMin, yMax);
}

void HistogramController::initBarParameters() {
  assert(activeSeriesMethod()(m_store, selectedSeries()));
  double xMin;
  double xMax;
  preinitXRangeParameters(&xMin, &xMax);
  m_store->setFirstDrawnBarAbscissa(xMin);
  double barWidth = m_histogramRange.xGridUnit();
  if (barWidth <= 0.0) {
    barWidth = 1.0;
  } else {
    // Round the bar width, as we convert from float to double
    const double precision = 7.0;  // FLT_EPS ~= 1e-7
    const double logBarWidth = OMG::IEEE754<double>::exponentBase10(barWidth);
    const double truncateFactor = std::pow(10.0, precision - logBarWidth);
    barWidth = std::round(barWidth * truncateFactor) / truncateFactor;
  }
  /* Start numberOfBars at k_maxNumberOfBars - 1 for extra margin in case of a
   * loss of precision. */
  int numberOfBars = HistogramRange::k_maxNumberOfBars;
  while (!HistogramParameterController::AuthorizedBarWidth(barWidth, xMin,
                                                           m_store) &&
         numberOfBars > 0) {
    numberOfBars--;
    barWidth = (xMax - xMin) / numberOfBars;
  }
  assert(HistogramParameterController::AuthorizedBarWidth(barWidth, xMin,
                                                          m_store));
  bool allValuesAreIntegers = true;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (allValuesAreIntegers && activeSeriesMethod()(m_store, i)) {
      allValuesAreIntegers = m_store->columnIsIntegersOnly(i, 0);
    }
  }
  if (allValuesAreIntegers) {
    // With integer values, the histogram is better with an integer bar width
    barWidth = std::ceil(barWidth);
    if (GlobalPreferences::SharedGlobalPreferences()->histogramOffset() ==
        CountryPreferences::HistogramsOffset::OnIntegerValues) {
      // Bars are offsetted right to center the bars around the labels.
      xMin -= barWidth / 2.0;
      m_store->setFirstDrawnBarAbscissa(xMin);
      m_histogramRange.setHistogramRange(
          m_histogramRange.xMin() - barWidth / 2.0, m_histogramRange.xMax());
    }
  }
  assert(barWidth > 0.0 && std::ceil((xMax - xMin) / barWidth) <=
                               HistogramRange::k_maxNumberOfBars);
  m_store->setBarWidth(barWidth);
}

void HistogramController::sanitizeSelectedIndex() {
  assert(activeSeriesMethod()(m_store, selectedSeries()));
  if (m_store->heightOfBarAtIndex(selectedSeries(), selectedIndex()) != 0) {
    return;
  }
  int numberOfBars = m_store->numberOfBars(selectedSeries());
  int previousIndex = selectedIndex();
  // search a bar with non null height left of the selected one
  while (m_store->heightOfBarAtIndex(selectedSeries(), selectedIndex()) == 0 &&
         selectedIndex() >= 0) {
    setSelectedIndex(selectedIndex() - 1);
  }
  if (selectedIndex() < 0) {
    // search a bar with non null height right of the selected one
    setSelectedIndex(previousIndex + 1);
    while (m_store->heightOfBarAtIndex(selectedSeries(), selectedIndex()) ==
               0 &&
           selectedIndex() < numberOfBars) {
      setSelectedIndex(selectedIndex() + 1);
    }
  }
  assert(selectedIndex() < numberOfBars);
  m_histogramRange.scrollToSelectedBarIndex(selectedSeries(), selectedIndex());
}

void HistogramController::updateHorizontalIndexAfterSelectingNewSeries(
    int previousSelectedSeries) {
  /* In the simple following case, when all bars are aligned, the selected
   * index should not change:
   *           _ _ _ _
   * series1: | | | | |
   *             ^ selected index = 1
   *           _ _ _ _
   * series2: | | | | |
   *             ^ select index 1 when moving down
   *
   * But in the case where bars do not start on the same spot, selected index
   * should be offsetted so that you always select the bar just above or under
   * the previously selected one:
   *           _ _ _ _
   * series1: | | | | |
   *             ^ selected index = 1
   *             _ _ _
   * series2:   | | | |
   *             ^ select index 0 when moving down
   *
   * At the end of this method, the selected index should be sanitized so that
   * an empty bar is never selected:
   *           _ _ _ _
   * series1: | | | | |
   *             ^ selected index = 1
   *           _     _
   * series2: | |_ _| |
   *           ^ select index 0 when moving down
   * */
  double startDifference =
      m_store->startOfBarAtIndex(previousSelectedSeries, 0) -
      m_store->startOfBarAtIndex(selectedSeries(), 0);
  setSelectedIndex(selectedIndex() +
                   static_cast<int>(startDifference / m_store->barWidth()));
  setSelectedIndex(std::max(
      std::min(selectedIndex(), m_store->numberOfBars(selectedSeries()) - 1),
      0));
  sanitizeSelectedIndex();
}

}  // namespace Statistics
