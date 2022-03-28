#include "plot_controller.h"
#include <poincare/print.h>
#include <poincare/preferences.h>
#include "../data/store_controller.h"

namespace Statistics {

PlotController::PlotController(Escher::Responder * parentResponder,
                               Escher::ButtonRowController * header,
                               Escher::Responder * tabController,
                               Escher::StackViewController * stackViewController,
                               Escher::ViewController * typeViewController,
                               Store * store,
                               int * selectedPointIndex,
                               int * selectedSeriesIndex) :
    MultipleDataViewController(parentResponder,
                               tabController,
                               header,
                               stackViewController,
                               typeViewController,
                               store,
                               selectedPointIndex,
                               selectedSeriesIndex),
    m_view(store, &m_curveView, &m_graphRange, &m_bannerView),
    // No bannerView given to the curve view because the display is handled here
    m_curveView(&m_graphRange, &m_cursor, &m_cursorView, store, this)  {
}

void PlotController::viewWillAppearBeforeReload() {
  float yMin, yMax, xMin, xMax;
  computeYBounds(&yMin, &yMax);
  computeXBounds(&xMin, &xMax);
  m_graphRange.calibrate(m_curveView.bounds().width(), m_curveView.bounds().height(), xMin, xMax, yMin, yMax);
  // Sanitize m_selectedBarIndex and cursor's position
  int series = selectedSeriesIndex();
  *m_selectedBarIndex = SanitizeIndex(*m_selectedBarIndex, totalValues(series));
  double x = valueAtIndex(selectedSeriesIndex(), *m_selectedBarIndex);
  double y = resultAtIndex(selectedSeriesIndex(), *m_selectedBarIndex);
  m_cursor.moveTo(x, x, y);
  m_curveView.reload();
}

bool PlotController::moveSelectionHorizontally(int deltaIndex) {
  int series = selectedSeriesIndex();
  assert(series >= 0);

  int nextIndex = SanitizeIndex(*m_selectedBarIndex + deltaIndex, totalValues(series));
  if (nextIndex != *m_selectedBarIndex) {
    *m_selectedBarIndex = nextIndex;
    // Compute coordinates
    double x = valueAtIndex(series, nextIndex);
    double y = resultAtIndex(series, nextIndex);
    m_cursor.moveTo(x, x, y);
    m_curveView.reload();
    return true;
  }
  return false;
}

bool PlotController::reloadBannerView() {
  int series = selectedSeriesIndex();
  if (series < 0) {
    return false;
  }
  KDCoordinate previousHeight = m_bannerView.minimalSizeForOptimalDisplay().height();

  *m_selectedBarIndex = SanitizeIndex(*m_selectedBarIndex, totalValues(series));

  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();
  char buffer[k_maxNumberOfCharacters] = "";

  // Display series name
  StoreController::FillSeriesName(series, buffer, false);
  m_bannerView.seriesName()->setText(buffer);

  // Display selected value
  Poincare::Print::customPrintf(
    buffer,
    k_maxNumberOfCharacters,
    "%s%s%*.*ed",
    I18n::translate(I18n::Message::StatisticsValue),
    I18n::translate(I18n::Message::StatisticsColonConvention),
    m_cursor.x(), displayMode, k_numberOfSignificantDigits);
  m_bannerView.value()->setText(buffer);

  // Display result value
  Poincare::Print::customPrintf(
    buffer,
    k_maxNumberOfCharacters,
    resultMessageTemplate(),
    I18n::translate(resultMessage()),
    I18n::translate(I18n::Message::StatisticsColonConvention),
    m_cursor.y(), displayMode, k_numberOfSignificantDigits);
  m_bannerView.result()->setText(buffer);

  m_bannerView.reload();
  return previousHeight != m_bannerView.minimalSizeForOptimalDisplay().height();
}

}
