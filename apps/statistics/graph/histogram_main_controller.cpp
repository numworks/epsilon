#include "histogram_main_controller.h"

#include <omg/float.h>
#include <omg/unreachable.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

namespace Statistics {

HistogramMainController::HistogramMainController(
    Escher::Responder* parentResponder, Escher::ButtonRowController* header,
    Escher::TabViewController* tabController,
    Escher::StackViewController* stackViewController,
    Escher::ViewController* typeViewController, Store* store,
    uint32_t* storeVersion)
    : Escher::ViewController(parentResponder),
      GraphButtonRowDelegate(header, stackViewController, this,
                             typeViewController),
      m_histogramRange(store),
      m_storeVersion(storeVersion),
      m_store(store),
      m_listController(this, m_store, &m_histogramRange, this),
      m_histogramParameterController(nullptr, store),
      m_view(m_listController.selectableListView()),
      m_parameterButton(
          this, I18n::Message::StatisticsGraphSettings,
          Escher::Invocation::Builder<HistogramMainController>(
              [](HistogramMainController* mainController, void* sender) {
                mainController->stackController()->push(
                    mainController->histogramParameterController());
                return true;
              },
              this),
          KDFont::Size::Small),
      m_tabController(tabController) {}

Escher::ButtonCell* HistogramMainController::buttonAtIndex(
    int index, Escher::ButtonRowController::Position position) const {
  assert(index == 0 || index == 1);
  return index == 0 ? GraphButtonRowDelegate::buttonAtIndex(index, position)
                    : const_cast<Escher::SimpleButtonCell*>(&m_parameterButton);
}

void HistogramMainController::viewWillAppear() {
  uint32_t storeChecksum = m_store->storeChecksum();
  if (*m_storeVersion != storeChecksum) {
    *m_storeVersion = storeChecksum;
    initBarParameters();
  }
  initRangeParameters();

  enterListView();
}

void HistogramMainController::enterHeaderView() {
  header()->setSelectedButton(0);
  // Take back the firstResponder ownership from the ButtonCell
  Escher::App::app()->setFirstResponder(this);
}

void HistogramMainController::exitHeaderView() {
  header()->setSelectedButton(-1);
}

void HistogramMainController::enterListView() {
  // Select or sanitize the series and the bar indices
  m_listController.processSeriesAndBarSelection();

  /* Update the model data displayed in the banner (this data depends on the
   * selected series and index), make the banner visible and recompute the graph
   * y range. */
  updateBannerView();
  m_view.setDisplayBanner(true);
  m_histogramRange.setYRange(computeYRange());

  // Highlight the selected series and bar
  m_listController.highlightSelectedSeries();
  m_listController.scrollAndHighlightHistogramBar(
      m_listController.selectedRow(), m_listController.selectedBarIndex());

  Escher::App::app()->setFirstResponder(&m_listController, true);
}

void HistogramMainController::exitListView() {
  m_view.setDisplayBanner(false);
  m_listController.unhighlightList();
}

bool HistogramMainController::handleEvent(Ion::Events::Event event) {
  if (isHeaderSelected()) {
    // Handle going up or down the header
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::Down) {
      exitHeaderView();
      enterListView();
      return true;
    }
    // Handle events on selected button, and navigation between header buttons
    return buttonAtIndex(header()->selectedButton(),
                         Escher::ButtonRowController::Position::Top)
        ->handleEvent(event);
  } else {
    // Handle going to the histogramParameter page
    if (event == Ion::Events::OK || event == Ion::Events::EXE ||
        event == Ion::Events::Toolbox) {
      stackController()->push(histogramParameterController());
      return true;
    }
    // Handle going up from the first list element
    if (event == Ion::Events::Up) {
      exitListView();
      enterHeaderView();
      return true;
    }
    return false;
  }
}

void HistogramMainController::didBecomeFirstResponder() {
  /* If the HistogramMainController page is being opened, the viewWillAppear
   * function has already been called, and the list has been initialized. */
  bool isListAlreadySelected = m_listController.selectedRow() >= 0;
  if (!isListAlreadySelected) {
    /* If the HistogramMainController took the first responder ownership from
     * another element in the page (for instance the TabViewController), the
     * header should be selected. */
    enterHeaderView();
  } else {
    enterListView();
  }
}

void HistogramMainController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::WillExit) {
    if (event.nextFirstResponder == m_tabController) {
      /* The tab controller is taking control, but the histogram view is still
       * visible. We restore the current subview to an unselected state. */
      assert(m_tabController != nullptr);
      if (isHeaderSelected()) {
        exitHeaderView();
        return;
      } else {
        exitListView();
        return;
      }
    }
  }
}

void HistogramMainController::updateBannerView() {
  KDCoordinate previousHeight =
      m_view.bannerView()->minimalSizeForOptimalDisplay().height();

  int precision =
      Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode =
      Poincare::Preferences::SharedPreferences()->displayMode();
  constexpr static int k_bufferSize =
      sizeof("Intervalle : [-1.2345ᴇ-123;-1.2345ᴇ-123[");  // longest case
  constexpr static int k_maxNumberOfGlyphs =
      Escher::Metric::MaxNumberOfSmallGlyphsInDisplayWidth;
  char buffer[k_bufferSize] = "";

  const int selectedSeries = m_listController.selectedSeries();
  const int selectedIndex = m_listController.selectedBarIndex();

  // Display series name
  m_store->tableName(selectedSeries, buffer, k_bufferSize);
  m_view.bannerView()->seriesName()->setText(buffer);

  // Display interval
  double lowerBound = m_store->startOfBarAtIndex(selectedSeries, selectedIndex);
  double upperBound = m_store->endOfBarAtIndex(selectedSeries, selectedIndex);
  Poincare::Print::CustomPrintfWithMaxNumberOfGlyphs(
      buffer, k_bufferSize, precision, k_maxNumberOfGlyphs,
      "%s%s[%*.*ed,%*.*ed%s", I18n::translate(I18n::Message::Interval),
      I18n::translate(I18n::Message::ColonConvention), lowerBound, displayMode,
      upperBound, displayMode,
      GlobalPreferences::SharedGlobalPreferences()->openIntervalChar(false));
  m_view.bannerView()->intervalView()->setText(buffer);

  // Display frequency
  double size = m_store->heightOfBarAtIndex(selectedSeries, selectedIndex);
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "%s%s%*.*ed",
                                I18n::translate(I18n::Message::Frequency),
                                I18n::translate(I18n::Message::ColonConvention),
                                size, displayMode, precision);
  m_view.bannerView()->frequencyView()->setText(buffer);

  // Display relative frequency
  double relativeFrequency = size / m_store->sumOfOccurrences(selectedSeries);
  Poincare::Print::CustomPrintf(
      buffer, k_bufferSize, "%s%s%*.*ed",
      I18n::translate(I18n::Message::RelativeFrequency),
      I18n::translate(I18n::Message::ColonConvention), relativeFrequency,
      displayMode, precision);
  m_view.bannerView()->relativeFrequencyView()->setText(buffer);

  if (previousHeight !=
      m_view.bannerView()->minimalSizeForOptimalDisplay().height()) {
    /* If the banner size has changed, the size of the list view has also
     * changed, so the whole view needs to be reloaded. */
    m_view.reload();
    /* With the change of list view size, we need to align the last list element
     * with the new bottom. */
    m_listController.selectableListView()->scrollToCell(
        m_listController.selectedSeries());

  } else {
    m_view.bannerView()->reload();
  }
}

Poincare::Range1D<double> HistogramMainController::activeSeriesRange() const {
  assert(m_store->hasActiveSeries());
  double minValue = DBL_MAX;
  double maxValue = -DBL_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsActive(i)) {
      minValue = std::min<double>(minValue, m_store->minValue(i));
      maxValue = std::max<double>(maxValue, m_store->maxValue(i));
    }
  }

  return Poincare::Range1D<double>{minValue, maxValue};
}

void HistogramMainController::initRangeParameters() {
  m_histogramRange.setXRange(computeXRange());
  m_histogramRange.setYRange(computeYRange());
}

void HistogramMainController::initBarParameters() {
  Poincare::Range1D<double> xRange = activeSeriesRange();
  m_histogramRange.setXRange(xRange.min(), xRange.max());

  m_store->setFirstDrawnBarAbscissa(xRange.min());
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
  while (!HistogramParameterController::AuthorizedBarWidth(
             barWidth, xRange.min(), m_store) &&
         numberOfBars > 0) {
    numberOfBars--;
    barWidth = (xRange.length()) / numberOfBars;
  }
  assert(HistogramParameterController::AuthorizedBarWidth(
      barWidth, xRange.min(), m_store));
  bool allValuesAreIntegers = true;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (allValuesAreIntegers && m_store->seriesIsActive(i)) {
      allValuesAreIntegers = m_store->columnIsIntegersOnly(i, 0);
    }
  }
  if (allValuesAreIntegers) {
    // With integer values, the histogram is better with an integer bar width
    barWidth = std::ceil(barWidth);
    if (GlobalPreferences::SharedGlobalPreferences()->histogramOffset() ==
        CountryPreferences::HistogramsOffset::OnIntegerValues) {
      // Bars are offsetted right to center the bars around the labels.
      xRange.setMinKeepingValid(xRange.min() - barWidth / 2.0);
      m_store->setFirstDrawnBarAbscissa(xRange.min());
      m_histogramRange.setXRange(m_histogramRange.xMin() - barWidth / 2.0,
                                 m_histogramRange.xMax());
    }
  }
  assert(barWidth > 0.0 && std::ceil((xRange.length()) / barWidth) <=
                               HistogramRange::k_maxNumberOfBars);
  m_store->setBarWidth(barWidth);
}

Poincare::Range1D<float> HistogramMainController::computeYRange() const {
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
  float viewHeight = static_cast<float>(m_listController.rowHeight());
  float yMin = yMax * bottomMargin / (bottomMargin - viewHeight);
  return {yMin, yMax};
}

Poincare::Range1D<float> HistogramMainController::computeXRange() const {
  double barWidth = m_store->barWidth();
  double xStart = m_store->firstDrawnBarAbscissa();

  Poincare::Range1D<double> xRange = activeSeriesRange();

  /* The range of bar at index barIndex is :
   * [xStart + barWidth * barIndex ; xStart + barWidth * (barIndex + 1)] */
  double barIndexMin =
      std::floor((xRange.min() - xStart) / barWidth + FLT_EPSILON);
  double barIndexMax =
      std::floor((xRange.max() - xStart) / barWidth + FLT_EPSILON);

  // barIndexMax is the right end of the last bar
  barIndexMax += 1.;

  /* If a bar is represented by less than one pixel, we cap xMax */
  barIndexMax = std::min(barIndexMax, barIndexMin + k_maxNumberOfBarsPerWindow);

  double xMin = xStart + barWidth * barIndexMin;
  double xMax = xStart + barWidth * barIndexMax;

  // TODO: Set the histogram range to double.
  float min = std::clamp(static_cast<float>(xMin),
                         -Poincare::Range1D<float>::k_maxFloat,
                         Poincare::Range1D<float>::k_maxFloat);
  float max = std::clamp(static_cast<float>(xMax),
                         -Poincare::Range1D<float>::k_maxFloat,
                         Poincare::Range1D<float>::k_maxFloat);

  return {min - HistogramRange::k_displayLeftMarginRatio * (max - min),
          max + HistogramRange::k_displayRightMarginRatio * (max - min)};
}

}  // namespace Statistics
