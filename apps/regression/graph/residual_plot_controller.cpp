#include "residual_plot_controller.h"

#include <assert.h>
#include <escher/stack_view_controller.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

namespace Regression {

ResidualPlotController::ResidualPlotController(
    Escher::Responder *parentResponder, Store *store)
    : Escher::ViewController(parentResponder),
      m_store(store),
      m_cursor(FLT_MAX),
      m_curveView(&m_range, &m_cursor, &m_bannerView, &m_cursorView, this),
      m_memoizedResidualStddev(NAN),
      m_selectedDotIndex(0),
      m_selectedSeriesIndex(0) {}

void ResidualPlotController::setSeries(int series) {
  m_selectedSeriesIndex = series;
  m_selectedDotIndex = 0;
}

void ResidualPlotController::updateCursor() {
  double x = xAtIndex(m_selectedDotIndex);
  double y = yAtIndex(m_selectedDotIndex);
  m_cursor.moveTo(x, x, y);
  m_cursorView.setColor(Escher::Palette::DataColor[m_selectedSeriesIndex]);

  const int significantDigits =
      Poincare::Preferences::sharedPreferences->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode =
      Poincare::Preferences::sharedPreferences->displayMode();
  constexpr size_t bufferSize =
      Shared::BannerView::k_maxLengthDisplayed - 2;  // -2 for "x="
  char buffer[bufferSize];

  Poincare::Print::CustomPrintf(buffer, bufferSize, "x=%*.*ed", x, displayMode,
                                significantDigits);
  m_bannerView.abscissaView()->setText(buffer);

  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%s%*.*ed",
                                I18n::translate(I18n::Message::Residual),
                                I18n::translate(I18n::Message::ColonConvention),
                                y, displayMode, significantDigits);
  m_bannerView.ordinateView()->setText(buffer);

  // TODO: Add translations
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s%s%*.*ed", "Residual standard deviation",
      I18n::translate(I18n::Message::ColonConvention), m_memoizedResidualStddev,
      displayMode, significantDigits);
  m_bannerView.stddevView()->setText(buffer);

  m_curveView.reload();
  m_bannerView.reload();
}

bool ResidualPlotController::moveHorizontally(
    OMG::HorizontalDirection direction) {
  int nextIndex = m_store->nextDot(m_selectedSeriesIndex, direction,
                                   m_selectedDotIndex, false);
  if (nextIndex == m_selectedDotIndex || nextIndex < 0) {
    return false;
  }
  assert(nextIndex < m_store->numberOfPairsOfSeries(m_selectedSeriesIndex));
  m_selectedDotIndex = nextIndex;
  updateCursor();
  return true;
}

bool ResidualPlotController::handleEvent(Ion::Events::Event event) {
  assert(m_store->seriesIsActive(m_selectedSeriesIndex));
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    static_cast<Escher::StackViewController *>(parentResponder())->pop();
    return true;
  }
  if (event == Ion::Events::Right || event == Ion::Events::Left) {
    return moveHorizontally(OMG::Direction(event));
  }
  return false;
}

void ResidualPlotController::viewWillAppear() {
  Escher::ViewController::viewWillAppear();
  m_curveView.setFocus(true);

  // Compute all data points to find the maximums and minimums
  double xMin = DBL_MAX;
  double xMax = DBL_MIN;
  double yMin = DBL_MAX;
  double yMax = DBL_MIN;
  int numberOfPairs = m_store->numberOfPairsOfSeries(m_selectedSeriesIndex);
  for (int i = 0; i < numberOfPairs; i++) {
    double x = xAtIndex(i);
    double y = yAtIndex(i);
    xMin = std::min(xMin, x);
    xMax = std::max(xMax, x);
    yMin = std::min(yMin, y);
    yMax = std::max(yMax, y);
  }
  m_range.calibrate(xMin, xMax, yMin, yMax, view()->bounds().height(),
                    m_bannerView.bounds().height());

  m_memoizedResidualStddev = m_store->residualStandardDeviation(
      m_selectedSeriesIndex,
      AppsContainerHelper::sharedAppsContainerGlobalContext());

  updateCursor();
}

}  // namespace Regression
