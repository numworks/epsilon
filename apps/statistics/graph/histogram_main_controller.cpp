#include "histogram_main_controller.h"

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
      m_storeVersion(storeVersion),
      m_store(store),
      m_histogramRange(store),
      m_tabController(tabController),
      m_listController(this, m_store, &m_histogramRange),
      m_histogramParameterController(nullptr, store),
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
      m_view(m_listController.selectableListView()) {}

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
  m_selectedSubview = SelectedSubview::Header;
  header()->selectFirstButton();
  // Take back the firstResponder ownership from the ButtonCell
  Escher::App::app()->setFirstResponder(this);
}

void HistogramMainController::exitHeaderView() {
  header()->unselectButtonRow();
}

void HistogramMainController::enterListView() {
  m_selectedSubview = SelectedSubview::List;

  m_listController.processSeriesAndBarSelection();
  m_listController.highlightRow(m_listController.selectedSeries());
  m_listController.highlightHistogramBar(m_listController.selectedSeries(),
                                         m_listController.selectedBarIndex());

  m_view.setBannerVisibility(true);
  updateBannerView();
}

void HistogramMainController::exitListView() {
  m_view.setBannerVisibility(false);
  m_listController.unhighlightList();
}

bool HistogramMainController::handleEvent(Ion::Events::Event event) {
  switch (m_selectedSubview) {
    case SelectedSubview::Header: {
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
    }
    case SelectedSubview::List: {
      // Handle going to the histogramParameter page
      if (event == Ion::Events::OK || event == Ion::Events::EXE ||
          event == Ion::Events::Toolbox) {
        stackController()->push(histogramParameterController());
        return true;
      }
      // Handle list navigation
      if (m_listController.handleEvent(event)) {
        updateBannerView();
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
    default:
      OMG::unreachable();
  }
}

void HistogramMainController::didEnterResponderChain(
    Responder* firstResponder) {
  switch (m_selectedSubview) {
    case SelectedSubview::Header: {
      enterHeaderView();
      return;
    }
    case SelectedSubview::List: {
      enterListView();
      return;
    }
    default:
      OMG::unreachable();
  }
}

void HistogramMainController::willExitResponderChain(
    Responder* nextFirstResponder) {
  if (nextFirstResponder == m_tabController) {
    /* The tab controller is taking control, but the histogram view is still
     * visible. We restore the current subview to an unselected state. */
    assert(m_tabController != nullptr);
    switch (m_selectedSubview) {
      case SelectedSubview::Header: {
        exitHeaderView();
        return;
      }
      case SelectedSubview::List: {
        exitListView();
        return;
      }
      default:
        OMG::unreachable();
    }
  }
}

void HistogramMainController::updateBannerView() {
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

  /* The banner size has changed, and this will change the heights of both the
   * list and the banner subviews */
  m_view.reload();

  // The histogram y range must be updated after the heights have changed
  initYRangeParameters();
}

Poincare::Range1D<double> HistogramMainController::activeSeriesRange() const {
  double minValue = DBL_MAX;
  double maxValue = -DBL_MAX;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (m_store->seriesIsActive(i)) {
      minValue = std::min<double>(minValue, m_store->minValue(i));
      maxValue = std::max<double>(maxValue, m_store->maxValue(i));
    }
  }
  return {minValue, maxValue};
}

void HistogramMainController::initBarParameters() {
  Poincare::Range1D<double> xRange = activeSeriesRange();
  m_histogramRange.setHistogramRange(xRange.min(), xRange.max());

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
    barWidth = (xRange.max() - xRange.min()) / numberOfBars;
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
      m_histogramRange.setHistogramRange(
          m_histogramRange.xMin() - barWidth / 2.0, m_histogramRange.xMax());
    }
  }
  assert(barWidth > 0.0 &&
         std::ceil((xRange.max() - xRange.min()) / barWidth) <=
             HistogramRange::k_maxNumberOfBars);
  m_store->setBarWidth(barWidth);
}

void HistogramMainController::initYRangeParameters() {
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
  float viewHeight = static_cast<float>(m_listController.histogramHeight());
  float yMin = yMax * bottomMargin / (bottomMargin - viewHeight);

  m_histogramRange.setYRange(yMin, yMax);
}

void HistogramMainController::initRangeParameters() {
  double barWidth = m_store->barWidth();
  double xStart = m_store->firstDrawnBarAbscissa();

  Poincare::Range1D<double> xRange = activeSeriesRange();
  m_histogramRange.setHistogramRange(xRange.min(), xRange.max());

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

  m_histogramRange.setHistogramRange(
      min - HistogramRange::k_displayLeftMarginRatio * (max - min),
      max + HistogramRange::k_displayRightMarginRatio * (max - min));

  initYRangeParameters();
}

}  // namespace Statistics
