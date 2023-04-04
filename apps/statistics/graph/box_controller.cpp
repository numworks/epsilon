#include "box_controller.h"

#include <poincare/print.h>

#include "../app.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Statistics {

BoxController::BoxController(Responder *parentResponder,
                             ButtonRowController *header,
                             TabViewController *tabController,
                             Escher::StackViewController *stackViewController,
                             Escher::ViewController *typeViewController,
                             Store *store)
    : MultipleDataViewController(parentResponder, tabController, header,
                                 stackViewController, typeViewController,
                                 store),
      m_view(store, this),
      m_boxParameterController(nullptr, store, this),
      m_parameterButton(this, I18n::Message::StatisticsGraphSettings,
                        Invocation::Builder<BoxController>(
                            [](BoxController *boxController, void *sender) {
                              boxController->stackController()->push(
                                  boxController->boxParameterController());
                              return true;
                            },
                            this),
                        KDFont::Size::Small) {}

AbstractButtonCell *BoxController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  return index == 0 ? GraphButtonRowDelegate::buttonAtIndex(index, position)
                    : const_cast<AbstractButtonCell *>(&m_parameterButton);
}

bool BoxController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Toolbox) {
    stackController()->push(boxParameterController());
    return true;
  }
  return MultipleDataViewController::handleEvent(event);
}

bool BoxController::moveSelectionHorizontally(
    OMG::HorizontalDirection direction) {
  return m_view.moveSelectionHorizontally(selectedSeries(), direction);
}

bool BoxController::reloadBannerView() {
  if (selectedSeries() < 0) {
    return false;
  }
  KDCoordinate previousHeight =
      m_view.bannerView()->minimalSizeForOptimalDisplay().height();

  int precision = Preferences::sharedPreferences->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode =
      Poincare::Preferences::sharedPreferences->displayMode();
  constexpr int k_bufferSize =
      1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  char buffer[k_bufferSize] = "";

  // Display series name
  StoreController::FillSeriesName(selectedSeries(), buffer, false);
  m_view.bannerView()->seriesName()->setText(buffer);

  // Display calculation
  int selectedBoxCalculation =
      m_view.plotViewForSeries(selectedSeries())->selectedBoxCalculation();
  double value = m_store->boxPlotCalculationAtIndex(selectedSeries(),
                                                    selectedBoxCalculation);
  Poincare::Print::CustomPrintf(
      buffer, k_bufferSize, "%s%s%*.*ed",
      I18n::translate(m_store->boxPlotCalculationMessageAtIndex(
          selectedSeries(), selectedBoxCalculation)),
      I18n::translate(I18n::Message::ColonConvention), value, displayMode,
      precision);
  m_view.bannerView()->calculationValue()->setText(buffer);

  m_view.bannerView()->reload();
  return previousHeight !=
         m_view.bannerView()->minimalSizeForOptimalDisplay().height();
}

void BoxController::updateHorizontalIndexAfterSelectingNewSeries(
    int previousSelectedSeries) {
  int resultIndex = -1;
  int previousNumberOfLowerOutliers =
      m_store->numberOfLowerOutliers(previousSelectedSeries);
  if (selectedIndex() < previousNumberOfLowerOutliers) {
    // Lower outliers were selected, select first lower outlier
    resultIndex = 0;
  } else if (selectedIndex() <
             previousNumberOfLowerOutliers + Store::k_numberOfQuantiles) {
    // Quartiles, max/min or median were selected, select same quantile
    resultIndex = selectedIndex() +
                  m_store->numberOfLowerOutliers(selectedSeries()) -
                  previousNumberOfLowerOutliers;
  } else {
    // Upper outliers were selected, select first upper outlier
    int numberOfUpperOutliers =
        m_store->numberOfUpperOutliers(selectedSeries());
    resultIndex = m_store->numberOfBoxPlotCalculations(selectedSeries()) -
                  (numberOfUpperOutliers == 0 ? 1 : numberOfUpperOutliers);
  }
  assert(resultIndex >= 0 &&
         resultIndex <= m_store->numberOfBoxPlotCalculations(selectedSeries()));
  setSelectedIndex(resultIndex);
}

}  // namespace Statistics
