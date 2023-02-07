#include "../app.h"
#include "graph_controller.h"
#include <apps/apps_container_helper.h>
#include <apps/exam_mode_configuration.h>
#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/helpers.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Shared::InteractiveCurveViewRange * interactiveRange, CurveViewCursor * cursor, int * selectedDotIndex, int * selectedCurveIndex, Store * store) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, interactiveRange, &m_view, cursor, I18n::Message::Regression, selectedCurveIndex),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(interactiveRange, store, m_cursor, &m_bannerView, &m_cursorView),
  m_store(store),
  m_graphOptionsController(this, inputEventHandlerDelegate, interactiveRange, m_store, m_cursor, this),
  m_curveSelectionController(this),
  m_selectedDotIndex(selectedDotIndex),
  m_selectedModelType((Model::Type)-1)
{
  interactiveRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

void GraphController::viewWillAppear() {
  m_cursorView.resetMemoization();
  /* At this point, some series might have been removed from the model. We need
   * to reinitialize the selected series index if the current selection is
   * either null (right after construction) or refering a removed series. */
  if (*m_selectedCurveIndex < 0 || *m_selectedCurveIndex >= numberOfCurves()) {
    *m_selectedCurveIndex = 0;
    m_selectedModelType = (Model::Type)-1;
  }

  /* Selected model type is memoized. The values differ in three cases:
   *  1) the very first time the graph view appears
   *  2) when the user selects another Model::Type for a series.
   *  3) selected series has been removed
   * where we decide to place the cursor at a default position. */
  Model::Type newSelectedModelType = m_store->seriesRegressionType(selectedSeriesIndex());
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

KDCoordinate GraphController::CurveSelectionController::nonMemoizedRowHeight(int j) {
  if (j < 0 || j >= numberOfRows()) {
    return 0;
  }
  return KDFont::GlyphHeight(KDFont::Size::Large) + Metric::CellTopMargin + Metric::CellBottomMargin;
}

void GraphController::CurveSelectionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int series = graphController()->seriesIndexFromCurveIndex(index);
  const char * name = Store::SeriesTitle(series);
  static_cast<CurveSelectionCellWithChevron *>(cell)->setColor(DoublePairStore::colorOfSeriesAtIndex(series));
  static_cast<CurveSelectionCellWithChevron *>(cell)->setLayout(LayoutHelper::String(name));
}

bool GraphController::buildRegressionExpression(char * buffer, size_t bufferSize, Model::Type modelType, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  double * coefficients = m_store->coefficientsForSeries(selectedSeriesIndex(), globalContext());
  Layout l = m_store->regressionModel(modelType)->equationLayout(coefficients, GlobalPreferences::sharedGlobalPreferences->yPredictedSymbol(), significantDigits, displayMode);
  int length = l.serializeForParsing(buffer, bufferSize);
  if (length >= static_cast<int>(bufferSize - 1) || length == 0) {
    return false;
  }
  length = SerializationHelper::ReplaceSystemParenthesesAndBracesByUserParentheses(buffer, length);
  return true;
}

// SimpleInteractiveCurveViewController

void GraphController::reloadBannerView() {
  const int selectedSeries = selectedSeriesIndex();
  const int significantDigits = Preferences::sharedPreferences->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences->displayMode();

  // If any coefficient is NAN, display that data is not suitable
  bool coefficientsAreDefined = m_store->coefficientsAreDefined(selectedSeries, globalContext());
  if (coefficientsAreDefined && *m_selectedDotIndex < 0 && selectedCurveIsScatterPlot()) {
    // Regression model has been removed, reinitialize cursor
    initCursorParameters();
  }
  bool displayMean = (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(selectedSeries));
  bool displayEquation = (*m_selectedDotIndex < 0);
  char buffer[k_bannerViewTextBufferSize];
  Model::Type modelType = m_store->seriesRegressionType(selectedSeries);
  if (displayEquation && coefficientsAreDefined && buildRegressionExpression(buffer, k_bannerViewTextBufferSize, modelType, significantDigits, displayMode)) {
    // Regression equation fits in the banner, display it
    m_bannerView.setDisplayParameters(true, false, false);
    m_bannerView.otherView()->setText(buffer);
  } else if (displayMean) {
    // Display MeanDot before x= and y=
    m_bannerView.setDisplayParameters(true, true, !coefficientsAreDefined);
    m_bannerView.otherView()->setText(I18n::translate(I18n::Message::MeanDot));
  } else if (modelType == Model::Type::None && !ExamModeConfiguration::statsDiagnosticsAreForbidden()) {
    // Display correlation coefficient
    m_bannerView.setDisplayParameters(true, false, !coefficientsAreDefined);
    Poincare::Print::CustomPrintf(buffer, k_bannerViewTextBufferSize, "r=%*.*ed", m_store->correlationCoefficient(selectedSeries), displayMode, significantDigits);
    m_bannerView.otherView()->setText(buffer);
  } else {
    // Nothing else to display
    m_bannerView.setDisplayParameters(false, false, !coefficientsAreDefined);
    m_bannerView.otherView()->setText("");
  }

  /* Use "x=..." or "xmean=..." (\xCC\x85 represents the combining overline ' ̅')
   * if the mean dot is selected. Same with y. */
  double x = displayMean ? m_store->meanOfColumn(selectedSeries, 0) : m_cursor->x();
  Poincare::Print::CustomPrintf(buffer, Shared::BannerView::k_maxLengthDisplayed - 2, "%*.*ed", x, displayMode, significantDigits); // -2 for "x="
  m_bannerView.abscissaValue()->setText(buffer);
  m_bannerView.abscissaSymbol()->setText(displayMean ? "x\xCC\x85=" : "x=");

  double y = displayMean ? m_store->meanOfColumn(selectedSeries, 1) : m_cursor->y();
  Poincare::Print::CustomPrintf(
    buffer, k_bannerViewTextBufferSize, "%s=%*.*ed",
    (displayMean ? "y\xCC\x85" : (displayEquation ? GlobalPreferences::sharedGlobalPreferences->yPredictedSymbol() : "y")),
    y, displayMode, significantDigits);
  m_bannerView.ordinateView()->setText(buffer);

  m_bannerView.reload();
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  const int selectedSeries = selectedSeriesIndex();
  double x;
  double y;
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(selectedSeries, direction, *m_selectedDotIndex, !selectedCurveIsScatterPlot());
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairsOfSeries(selectedSeries)) {
      x = m_store->get(selectedSeries, 0, dotSelected);
      y = m_store->get(selectedSeries, 1, dotSelected);
    } else if (dotSelected == m_store->numberOfPairsOfSeries(selectedSeries)) {
      assert(!selectedCurveIsScatterPlot());
      x = m_store->meanOfColumn(selectedSeries, 0);
      y = m_store->meanOfColumn(selectedSeries, 1);
    } else {
      return false;
    }
    *m_selectedDotIndex = dotSelected;
  } else {
    double step = direction * scrollSpeed * static_cast<double>(interactiveCurveViewRange()->xGridUnit())/static_cast<double>(k_numberOfCursorStepsInGradUnit);
    x = m_cursor->x() + step;
    y = m_store->yValueForXValue(selectedSeries, x, globalContext());
  }
  m_cursor->moveTo(x, x, y);
  return true;
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() const {
  return App::app()->graphRange();
}

void GraphController::openMenuForCurveAtIndex(int curveIndex) {
  if (*m_selectedCurveIndex != curveIndex) {
    *m_selectedCurveIndex = curveIndex;
    Coordinate2D<double> xy = xyValues(curveIndex, m_cursor->t(), textFieldDelegateApp()->localContext());
    m_cursor->moveTo(m_cursor->t(), xy.x1(), xy.x2());
  }
  if (selectedCurveIsScatterPlot()) {
    // Push regression controller directly
    RegressionController * controller = App::app()->regressionController();
    controller->setSeries(selectedSeriesIndex());
    controller->setDisplayedFromDataTab(false);
    stackController()->push(controller);
  } else {
    // Reset selected row. It is preserved when navigating in its submenus
    m_graphOptionsController.selectRow(0);
    stackController()->push(&m_graphOptionsController);
  }
}

// InteractiveCurveViewController
void GraphController::initCursorParameters(bool ignoreMargins) {
  const int selectedSeries = selectedSeriesIndex();
  double x, y;
  if (selectedCurveIsScatterPlot()) {
    x = m_store->get(selectedSeries, 0, 0);
    y = m_store->get(selectedSeries, 1, 0);
    *m_selectedDotIndex = 0;
  } else {
    x = m_store->meanOfColumn(selectedSeries, 0);
    y = m_store->meanOfColumn(selectedSeries, 1);
    *m_selectedDotIndex = m_store->numberOfPairsOfSeries(selectedSeries);
  }
  m_cursor->moveTo(x, x, y);
}

bool GraphController::selectedModelIsValid() const {
  uint8_t numberOfPairs = m_store->numberOfPairsOfSeries(selectedSeriesIndex());
  return *m_selectedDotIndex < numberOfPairs || (*m_selectedDotIndex == numberOfPairs && !selectedCurveIsScatterPlot());
}

Poincare::Coordinate2D<double> GraphController::selectedModelXyValues(double t) const {
  assert(selectedModelIsValid());
  const int selectedSeries = selectedSeriesIndex();
  if (*m_selectedDotIndex == -1) {
    return xyValues(*m_selectedCurveIndex, t, globalContext());
  } else if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(selectedSeries) && !selectedCurveIsScatterPlot()) {
    return Coordinate2D<double>(m_store->meanOfColumn(selectedSeries, 0), m_store->meanOfColumn(selectedSeries, 1));
  }
  return Coordinate2D<double>(m_store->get(selectedSeries, 0, *m_selectedDotIndex), m_store->get(selectedSeries, 1, *m_selectedDotIndex));
}

bool GraphController::moveCursorVertically(int direction) {
  int selectedSeries = selectedSeriesIndex();
  Poincare::Context * context = globalContext();
  double x = m_cursor->x();
  double y = m_cursor->y();

  // Find the closest regression
  int selectedRegressionCurve = *m_selectedDotIndex == -1 ? *m_selectedCurveIndex : -1;
  int closestRegressionCurve = closestCurveIndexVertically(direction > 0, selectedRegressionCurve, context);

  // Find the closest dot
  int closestDotSeries = -1;
  int dotSelected = m_store->closestVerticalDot(direction, x, y, selectedSeries, *m_selectedDotIndex, &closestDotSeries, context);

  // Choose between selecting the regression or the dot
  bool validRegression = closestRegressionCurve > -1;
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
      double regressionDistanceY = std::fabs(m_store->yValueForXValue(closestRegressionCurve, x, context) - y);
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
    if (selectedSeries != closestRegressionCurve) {
      *m_selectedCurveIndex = curveIndexFromSeriesIndex(closestRegressionCurve);
      selectedSeries = selectedSeriesIndex();
      // Reload so that the selected series is on top
      m_view.reload(false, true);
    }
    *m_selectedDotIndex = -1;
    setRoundCrossCursorView();
    m_cursor->moveTo(x, x, m_store->yValueForXValue(selectedSeries, x, context));
    setAbscissaInputAsFirstResponder();
    return true;
  }

  if (validDot) {
    // Select the dot
    if (selectedSeries != closestDotSeries) {
      *m_selectedCurveIndex = curveIndexFromSeriesIndex(closestDotSeries);
      selectedSeries = selectedSeriesIndex();
      // Reload so that the selected series is on top
      m_view.reload(false, true);
    }
    *m_selectedDotIndex = dotSelected;
    setRoundCrossCursorView();
    if (dotSelected == m_store->numberOfPairsOfSeries(selectedSeries)) {
      assert(!selectedCurveIsScatterPlot());
      // Select the mean dot
      double x = m_store->meanOfColumn(selectedSeries, 0);
      double y = m_store->meanOfColumn(selectedSeries, 1);
      m_cursor->moveTo(x, x, y);
    } else {
      // Select a data point dot
      double x = m_store->get(selectedSeries, 0, *m_selectedDotIndex);
      double y = m_store->get(selectedSeries, 1, *m_selectedDotIndex);
      m_cursor->moveTo(x, x, y);
    }
    // abscissa input must resolve first responder
    Container::activeApp()->setFirstResponder(this);
    return true;
  }

  // There was no suitable selection
  return false;
}

Coordinate2D<double> GraphController::xyValues(int curveIndex, double t, Poincare::Context * context, int subCurveIndex) const {
  int seriesIndex = seriesIndexFromCurveIndex(curveIndex);
  return Coordinate2D<double>(t, m_store->yValueForXValue(seriesIndex, t, context));
}

bool GraphController::suitableYValue(double y) const {
  return interactiveCurveViewRange()->yMin() <= y && y <= interactiveCurveViewRange()->yMax();
}

void GraphController::setRoundCrossCursorView() {
  /* At this point, the model (selected series and dot indices) should be up
   * to date. */
  bool round = *m_selectedDotIndex < 0;
  m_cursorView.setIsRing(!round);
  assert(selectedSeriesIndex() < static_cast<int>(Palette::numberOfDataColors()));
  m_cursorView.setColor(Palette::DataColor[selectedSeriesIndex()]);
}

Range2D GraphController::optimalRange(bool computeX, bool computeY, Range2D originalRange) const {
  Range1D xRange, yRange;
  const int nbOfCurves = numberOfCurves();
  for (int curve = 0; curve < nbOfCurves; curve++) {
    int series = seriesIndexFromCurveIndex(curve);
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
