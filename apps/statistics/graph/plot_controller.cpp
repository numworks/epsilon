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

void PlotController::viewWillAppear() {
  MultipleDataViewController::viewWillAppear();
  float yMin, yMax, xMin, xMax;
  computeYBounds(&yMin, &yMax);
  computeXBounds(&xMin, &xMax);
  m_graphRange.calibrate(m_curveView.bounds().width(), m_curveView.bounds().height(), xMin, xMax, yMin, yMax);
}

bool PlotController::moveSelectionHorizontally(int deltaIndex) {
  int series = selectedSeriesIndex();
  assert(series >= 0);
  /* TODO : Avoid having to sort the index here by memoizing sortedIndex or
   * totalValues. At the very least, building the sorted index is unecessary
   * on NormalProbabilityControllers. */
  int sortedIndex[Store::k_maxNumberOfPairs];
  m_store->buildSortedIndex(series, sortedIndex);

  int nextIndex = SanitizeIndex(*m_selectedBarIndex + deltaIndex, totalValues(series, sortedIndex));
  if (nextIndex != *m_selectedBarIndex) {
    *m_selectedBarIndex = nextIndex;
    // TODO : Add continuous curve scrolling
    // Compute coordinates
    double x = valueAtIndex(series, sortedIndex, nextIndex);
    double y = resultAtIndex(series, sortedIndex, nextIndex);
    m_cursor.moveTo(x, x, y);
    m_curveView.reload();
    reloadBannerView();
    return true;
  }
  return false;
}

void PlotController::reloadBannerView() {
  int series = selectedSeriesIndex();
  if (series < 0) {
    return;
  }
  int sortedIndex[Store::k_maxNumberOfPairs];
  m_store->buildSortedIndex(series, sortedIndex);

  *m_selectedBarIndex = SanitizeIndex(*m_selectedBarIndex, totalValues(series, sortedIndex));

  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();
  char buffer[k_maxNumberOfCharacters] = "";

  // Display series name
  StoreController::FillSeriesName(series, buffer, false);
  m_bannerView.seriesName()->setText(buffer);

  // Display selected value
  double value = valueAtIndex(series, sortedIndex, *m_selectedBarIndex);
  Poincare::Print::customPrintf(buffer, k_maxNumberOfCharacters, "%s : %*.*ed",
    I18n::translate(I18n::Message::StatisticsValue), value, displayMode, k_numberOfSignificantDigits);
  m_bannerView.value()->setText(buffer);

  // Display cumulated frequency
  double frequency = resultAtIndex(series, sortedIndex, *m_selectedBarIndex);
  Poincare::Print::customPrintf(buffer, k_maxNumberOfCharacters, resultMessageTemplate(),
    I18n::translate(resultMessage()), frequency, displayMode, k_numberOfSignificantDigits);
  m_bannerView.result()->setText(buffer);

  m_bannerView.reload();
}

}
