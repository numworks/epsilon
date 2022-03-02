#include "plot_controller.h"
#include <poincare/print.h>
#include <poincare/preferences.h>

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
                               selectedSeriesIndex) {
}

bool PlotController::moveSelectionHorizontally(int deltaIndex) {
  int series = selectedSeriesIndex();
  assert(series >= 0);
  /* TODO : Avoid having to sort the index here by memoizing sortedIndex or
   * totalValues. At the very least, building the sorted index is unecessary
   * on NormalProbabilityControllers. */
  int sortedIndex[Store::k_maxNumberOfPairs];
  m_store->buildSortedIndex(series, sortedIndex);

  int nextIndex = SanitizeIndex(*m_selectedBarIndex + deltaIndex, plotView()->plotCurveView()->totalValues(series, sortedIndex));
  if (nextIndex != *m_selectedBarIndex) {
    *m_selectedBarIndex = nextIndex;
    plotView()->moveCursorTo(nextIndex, series);
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

  *m_selectedBarIndex = SanitizeIndex(*m_selectedBarIndex, plotView()->plotCurveView()->totalValues(series, sortedIndex));

  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();
  char buffer[k_maxNumberOfCharacters] = "";

  // Display series name
  char seriesChar = '0' + series + 1;
  Poincare::Print::customPrintf(buffer, k_maxNumberOfCharacters, "V%c/N%c", seriesChar, seriesChar);
  plotView()->bannerView()->seriesName()->setText(buffer);

  // Display selected value
  double value = plotView()->plotCurveView()->valueAtIndex(series, sortedIndex, *m_selectedBarIndex);
  Poincare::Print::customPrintf(buffer, k_maxNumberOfCharacters, "%s : %*.*ed",
    I18n::translate(I18n::Message::StatisticsValue), value, displayMode, k_numberOfSignificantDigits);
  plotView()->bannerView()->value()->setText(buffer);

  // Display cumulated frequency
  double frequency = plotView()->plotCurveView()->resultAtIndex(series, sortedIndex, *m_selectedBarIndex);
  Poincare::Print::customPrintf(buffer, k_maxNumberOfCharacters, resultMessageTemplate(),
    I18n::translate(resultMessage()), frequency, displayMode, k_numberOfSignificantDigits);
  plotView()->bannerView()->result()->setText(buffer);

  plotView()->bannerView()->reload();
}

}
