#include "../app.h"
#include "graph_controller.h"
#include <apps/apps_container_helper.h>
#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/layout_helper.h>
#include <poincare/helpers.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Store * store, CurveViewCursor * cursor, int * selectedDotIndex, int * selectedSeriesIndex) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, store, &m_view, cursor),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(store, m_cursor, &m_bannerView, &m_cursorView),
  m_store(store),
  m_graphOptionsController(this, inputEventHandlerDelegate, m_store, m_cursor, this),
  m_seriesSelectionController(this),
  m_calculusButton(this, I18n::Message::Regression, calculusButtonInvocation(), KDFont::Size::Small),
  m_selectedDotIndex(selectedDotIndex),
  m_selectedSeriesIndex(selectedSeriesIndex),
  m_selectedModelType((Model::Type)-1)
{
  m_store->setDelegate(this);
}

bool GraphController::isEmpty() const {
  return !m_store->hasValidSeries();
}

I18n::Message GraphController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

void GraphController::viewWillAppear() {
  m_cursorView.resetMemoization();
  /* At this point, some series might have been removed from the model. We need
   * to reinitialize the selected series index if the current selection is
   * either null (right after construction) or refering a removed series. */
  if (*m_selectedSeriesIndex < 0 || !m_store->seriesIsValid(*m_selectedSeriesIndex)) {
    *m_selectedSeriesIndex = m_store->indexOfKthValidSeries(0);
    m_selectedModelType = (Model::Type)-1;
  }

  /* Selected model type is memoized. The values differ in three cases:
   *  1) the very first time the graph view appears
   *  2) when the user selects another Model::Type for a series.
   *  3) selected series has been removed
   * where we decide to place the cursor at a default position. */
  Model::Type newSelectedModelType = m_store->seriesRegressionType(*m_selectedSeriesIndex);
  if (m_selectedModelType != newSelectedModelType) {
    m_selectedModelType = newSelectedModelType;
    initCursorParameters();
  }

  /* The following
   *   - calls initCursorParameters() if necessary,
   *   - reloads the bannerView and the curveView. */
  InteractiveCurveViewController::viewWillAppear();

  /* Since *m_selectedDotIndex is altered by initCursorParameters(),
   * the following must absolutely come at the end. */
  setRoundCrossCursorView();
}

void GraphController::didBecomeFirstResponder() {
  if (*m_selectedDotIndex == -1 && curveView()->hasFocus()) {
    setAbscissaInputAsFirstResponder();
  }
  Shared::InteractiveCurveViewController::didBecomeFirstResponder();
}

void GraphController::setAbscissaInputAsFirstResponder() {
  m_bannerView.abscissaValue()->setParentResponder(this);
  m_bannerView.abscissaValue()->setDelegates(textFieldDelegateApp(), this);
  Container::activeApp()->setFirstResponder(m_bannerView.abscissaValue());
}

Poincare::Context * GraphController::globalContext() const {
  return AppsContainerHelper::sharedAppsContainerGlobalContext();
}

// Private

KDCoordinate GraphController::SeriesSelectionController::nonMemoizedRowHeight(int j) {
  if (j < 0 || j >= numberOfRows()) {
    return 0;
  }
  return KDFont::GlyphHeight(KDFont::Size::Large) + Metric::CellTopMargin + Metric::CellBottomMargin;
}

void GraphController::SeriesSelectionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int series = graphController()->m_store->indexOfKthValidSeries(index);
  const char * name = Store::SeriesTitle(series);
  static_cast<CurveSelectionCellWithChevron *>(cell)->setColor(DoublePairStore::colorOfSeriesAtIndex(series));
  static_cast<CurveSelectionCellWithChevron *>(cell)->setLayout(LayoutHelper::String(name));
}

bool GraphController::buildRegressionExpression(char * buffer, size_t bufferSize, Model::Type modelType, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  int length = Poincare::Print::SafeCustomPrintf(buffer, bufferSize, "%s=", GlobalPreferences::sharedGlobalPreferences()->yPredictedSymbol());
  if (length >= static_cast<int>(bufferSize)) {
    return false;
  }
  buffer += length;
  bufferSize -= length;

  double * coefficients = m_store->coefficientsForSeries(*m_selectedSeriesIndex, globalContext());
  length = m_store->regressionModel(modelType)->buildEquationTemplate(buffer, bufferSize, coefficients, significantDigits, displayMode);
  return length < static_cast<int>(bufferSize);
}

// SimpleInteractiveCurveViewController

void GraphController::reloadBannerView() {
  const int significantDigits = Preferences::sharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();

  // If any coefficient is NAN, display that data is not suitable
  bool coefficientsAreDefined = m_store->coefficientsAreDefined(*m_selectedSeriesIndex, globalContext());
  if (coefficientsAreDefined && *m_selectedDotIndex < 0 && selectedSeriesIsScatterPlot()) {
    // Regression model has been removed, reinitialize cursor
    initCursorParameters();
  }
  bool displayMean = (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex));
  bool displayEquation = (*m_selectedDotIndex < 0);
  char buffer[k_bannerViewTextBufferSize];
  Model::Type modelType = m_store->seriesRegressionType(*m_selectedSeriesIndex);
  if (displayEquation && coefficientsAreDefined && buildRegressionExpression(buffer, k_bannerViewTextBufferSize, modelType, significantDigits, displayMode)) {
    // Regression equation fits in the banner, display it
    m_bannerView.setDisplayParameters(true, false, false);
    m_bannerView.otherView()->setText(buffer);
  } else if (displayMean) {
    // Display MeanDot before x= and y=
    m_bannerView.setDisplayParameters(true, true, !coefficientsAreDefined);
    m_bannerView.otherView()->setText(I18n::translate(I18n::Message::MeanDot));
  } else if (modelType == Model::Type::None) {
    // Display correlation coefficient
    m_bannerView.setDisplayParameters(true, false, !coefficientsAreDefined);
    Poincare::Print::CustomPrintf(buffer, k_bannerViewTextBufferSize, "r=%*.*ed", m_store->correlationCoefficient(*m_selectedSeriesIndex), displayMode, significantDigits);
    m_bannerView.otherView()->setText(buffer);
  } else {
    // Nothing else to display
    m_bannerView.setDisplayParameters(false, false, !coefficientsAreDefined);
    m_bannerView.otherView()->setText("");
  }

  /* Use "x=..." or "xmean=..." (\xCC\x85 represents the combining overline ' ̅')
   * if the mean dot is selected. Same with y. */
  double x = displayMean ? m_store->meanOfColumn(*m_selectedSeriesIndex, 0) : m_cursor->x();
  Poincare::Print::CustomPrintf(buffer, Shared::BannerView::k_maxLengthDisplayed - 2, "%*.*ed", x, displayMode, significantDigits); // -2 for "x="
  m_bannerView.abscissaValue()->setText(buffer);
  m_bannerView.abscissaSymbol()->setText(displayMean ? "x\xCC\x85=" : "x=");

  double y = displayMean ? m_store->meanOfColumn(*m_selectedSeriesIndex, 1) : m_cursor->y();
  Poincare::Print::CustomPrintf(
    buffer, k_bannerViewTextBufferSize, "%s=%*.*ed",
    (displayMean ? "y\xCC\x85" : (displayEquation ? GlobalPreferences::sharedGlobalPreferences()->yPredictedSymbol() : "y")),
    y, displayMode, significantDigits);
  m_bannerView.ordinateView()->setText(buffer);

  m_bannerView.reload();
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  double x;
  double y;
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(*m_selectedSeriesIndex, direction, *m_selectedDotIndex, !selectedSeriesIsScatterPlot());
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      x = m_store->get(*m_selectedSeriesIndex, 0, dotSelected);
      y = m_store->get(*m_selectedSeriesIndex, 1, dotSelected);
    } else if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      assert(!selectedSeriesIsScatterPlot());
      x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
      y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
    } else {
      return false;
    }
    *m_selectedDotIndex = dotSelected;
  } else {
    double step = direction * scrollSpeed * static_cast<double>(m_store->xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
    x = m_cursor->x() + step;
    y = yValue(*m_selectedSeriesIndex, x, globalContext());
  }
  m_cursor->moveTo(x, x, y);
  return true;
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return m_store;
}

AbstractPlotView * GraphController::curveView() {
  return &m_view;
}

bool GraphController::openMenuForCurveAtIndex(int index) {
  int activeIndex = m_store->indexOfKthValidSeries(index);
  if (*m_selectedSeriesIndex != activeIndex) {
    *m_selectedSeriesIndex = activeIndex;
    Coordinate2D<double> xy = xyValues(activeIndex, m_cursor->t(), textFieldDelegateApp()->localContext());
    m_cursor->moveTo(m_cursor->t(), xy.x1(), xy.x2());
  }
  if (selectedSeriesIsScatterPlot()) {
    // Push regression controller directly
    RegressionController * controller = App::app()->regressionController();
    controller->setSeries(*m_selectedSeriesIndex);
    controller->setDisplayedFromDataTab(false);
    stackController()->push(controller);
  } else {
    // Reset selected row. It is preserved when navigating in its submenus
    m_graphOptionsController.selectRow(0);
    stackController()->push(&m_graphOptionsController);
  }
  return true;
}

// InteractiveCurveViewController
void GraphController::initCursorParameters() {
  double x, y;
  if (selectedSeriesIsScatterPlot()) {
    x = m_store->get(*m_selectedSeriesIndex, 0, 0);
    y = m_store->get(*m_selectedSeriesIndex, 1, 0);
    *m_selectedDotIndex = 0;
  } else {
    x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
    y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
    *m_selectedDotIndex = m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex);
  }
  m_cursor->moveTo(x, x, y);
}

bool GraphController::selectedModelIsValid() const {
  if (!m_store->seriesIsValid(*m_selectedSeriesIndex)) {
    return false;
  }
  uint8_t numberOfPairs = m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex);
  return *m_selectedDotIndex < numberOfPairs || (*m_selectedDotIndex == numberOfPairs && !selectedSeriesIsScatterPlot());
}

Poincare::Coordinate2D<double> GraphController::selectedModelXyValues(double t) const {
  assert(selectedModelIsValid());
  if (*m_selectedDotIndex == -1) {
    return xyValues(*m_selectedSeriesIndex, t, globalContext());
  } else if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex) && !selectedSeriesIsScatterPlot()) {
    return Coordinate2D<double>(m_store->meanOfColumn(*m_selectedSeriesIndex, 0), m_store->meanOfColumn(*m_selectedSeriesIndex, 1));
  }
  return Coordinate2D<double>(m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex), m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex));
}

bool GraphController::moveCursorVertically(int direction) {
  Poincare::Context * context = globalContext();
  double x = m_cursor->x();
  double y = m_cursor->y();

  // Find the closest regression
  int selectedRegressionIndex = *m_selectedDotIndex == -1 ? *m_selectedSeriesIndex : -1;
  int closestRegressionSeries = closestCurveIndexVertically(direction > 0, selectedRegressionIndex, context);

  // Find the closest dot
  int closestDotSeries = -1;
  int dotSelected = m_store->closestVerticalDot(direction, x, y, *m_selectedSeriesIndex, *m_selectedDotIndex, &closestDotSeries, context);

  // Choose between selecting the regression or the dot
  bool validRegression = closestRegressionSeries > -1;
  bool validDot = dotSelected >= 0 && dotSelected <= m_store->numberOfPairsOfSeries(closestDotSeries);
  if (validRegression && validDot) {
    /* Compare the abscissa distances to select either the dot or the
     * regression. If they are equal, compare the ordinate distances. */
    double dotDistanceX = -1;
    if (dotSelected == m_store->numberOfPairsOfSeries(closestDotSeries)) {
      dotDistanceX = std::fabs(m_store->meanOfColumn(closestDotSeries, 0) - x);
    } else {
      dotDistanceX = std::fabs(m_store->get(closestDotSeries, 0, dotSelected) - x);
    }
    if (dotDistanceX != 0) {
      /* The regression X distance to the point is 0, so it is closer than the
       * dot. */
      validDot = false;
    } else {
      // Compare the y distances
      double regressionDistanceY = std::fabs(yValue(closestRegressionSeries, x, context) - y);
      double dotDistanceY = (dotSelected == m_store->numberOfPairsOfSeries(closestDotSeries)) ?
        std::fabs(m_store->meanOfColumn(closestDotSeries, 1) - y) :
        std::fabs(m_store->get(closestDotSeries, 1, dotSelected) - y);
      if (regressionDistanceY <= dotDistanceY) {
        validDot = false;
      } else {
        validRegression = false;
      }
    }
  }

  assert(!validDot || !validRegression);

  /* The model should be up to date before setting the cursor view. */

  if (validRegression) {
    // Select the regression
    if (*m_selectedSeriesIndex != closestRegressionSeries) {
      *m_selectedSeriesIndex = closestRegressionSeries;
      // Reload so that the selected series is on top
      m_view.reload(false, true);
    }
    *m_selectedDotIndex = -1;
    setRoundCrossCursorView();
    m_cursor->moveTo(x, x, yValue(*m_selectedSeriesIndex, x, context));
    setAbscissaInputAsFirstResponder();
    return true;
  }

  if (validDot) {
    // Select the dot
    if (*m_selectedSeriesIndex != closestDotSeries) {
      *m_selectedSeriesIndex = closestDotSeries;
      // Reload so that the selected series is on top
      m_view.reload(false, true);
    }
    *m_selectedDotIndex = dotSelected;
    setRoundCrossCursorView();
    if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      assert(!selectedSeriesIsScatterPlot());
      // Select the mean dot
      double x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
      double y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
      m_cursor->moveTo(x, x, y);
    } else {
      // Select a data point dot
      double x = m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex);
      double y = m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex);
      m_cursor->moveTo(x, x, y);
    }
    Container::activeApp()->setFirstResponder(this); // abscissa input must resolve first responder
    return true;
  }

  // There was no suitable selection
  return false;
}

int GraphController::selectedCurveIndex(bool relativeIndex) const {
  int res = *m_selectedSeriesIndex;
  if (res < 0) {
    return -1;
  }
  if (!relativeIndex) {
    return res;
  }
  for (int i = 0; i < *m_selectedSeriesIndex; i++) {
    if (!m_store->seriesIsValid(i)) {
      res--;
    }
  }
  return res;
}

bool GraphController::closestCurveIndexIsSuitable(int newIndex, int currentIndex, int newSubIndex, int currentSubIndex) const {
  return InteractiveCurveViewController::closestCurveIndexIsSuitable(newIndex, currentIndex, newSubIndex, currentSubIndex) && m_store->seriesIsValid(newIndex);
}

Coordinate2D<double> GraphController::xyValues(int curveIndex, double x, Poincare::Context * context, int subCurveIndex) const {
  return Coordinate2D<double>(x, yValue(curveIndex, x, context));
}

double GraphController::yValue(int curveIndex, double x, Poincare::Context * context) const {
  return m_store->yValueForXValue(curveIndex, x, context);
}

bool GraphController::suitableYValue(double y) const {
  return m_store->yMin() <= y && y <= m_store->yMax();
}

int GraphController::numberOfCurves() const {
  return Store::k_numberOfSeries;
}

void GraphController::setRoundCrossCursorView() {
  /* At this point, the model (selected series and dot indices) should be up
   * to date. */
  bool round = *m_selectedDotIndex < 0;
  m_cursorView.setIsRing(!round);
  assert(*m_selectedSeriesIndex < static_cast<int>(Palette::numberOfDataColors()));
  m_cursorView.setColor(Palette::DataColor[*m_selectedSeriesIndex]);
}

Range2D GraphController::optimalRange(bool computeX, bool computeY, Range2D originalRange) const {
  Range1D xRange, yRange;
  for (int series = 0; series < Store::k_numberOfSeries; series++) {
    if (!m_store->seriesIsValid(series)) {
      continue;
    }

    int numberOfPairs = m_store->numberOfPairsOfSeries(series);
    for (int pair = 0; pair < numberOfPairs; pair++) {
      float x = m_store->get(series, 0, pair);
      xRange.extend(x, k_maxFloat);
      if (computeX || (originalRange.xMin() <= x && x <= originalRange.xMax())) {
        float y = m_store->get(series, 1, pair);
        yRange.extend(y, k_maxFloat);
      }
    }
  }

  Range2D result(computeX ? xRange : *originalRange.x(), computeY ? yRange : *originalRange.y());
  return Zoom::Sanitize(result, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
}

}
