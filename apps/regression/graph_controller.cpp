#include "app.h"
#include "graph_controller.h"
#include "../apps_container.h"
#include "../shared/function_banner_delegate.h"
#include "../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/layout_helper.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Store * store, CurveViewCursor * cursor, uint32_t * rangeVersion, int * selectedDotIndex, int * selectedSeriesIndex) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, store, &m_view, cursor, rangeVersion),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(store, m_cursor, &m_bannerView, &m_crossCursorView),
  m_store(store),
  m_graphOptionsController(this, inputEventHandlerDelegate, m_store, m_cursor, this),
  m_seriesSelectionController(this),
  m_calculusButton(this, I18n::Message::Regression, calculusButtonInvocation(), KDFont::SmallFont),
  m_selectedDotIndex(selectedDotIndex),
  m_selectedSeriesIndex(selectedSeriesIndex)
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_modelType[i] = (Model::Type) -1;
  }
  m_store->setDelegate(this);
}

bool GraphController::isEmpty() const {
  return !m_store->hasValidSeries();
}

I18n::Message GraphController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

void GraphController::viewWillAppear() {
#ifdef GRAPH_CURSOR_SPEEDUP
  m_roundCursorView.resetMemoization();
#endif
  /* At this point, some series might have been removed from the model. We need
   * to reinitialize the selected series index if the current selection is
   * either null (right after construction) or refering a removed series. */
  if (*m_selectedSeriesIndex < 0 || !m_store->seriesIsValid(*m_selectedSeriesIndex)) {
    *m_selectedSeriesIndex = m_store->indexOfKthValidSeries(0);
  }

  Model::Type previousSelectedRegressionType = m_modelType[*m_selectedSeriesIndex];

  /* Equalize the Model::Type of each series between the GraphController and
   * the Store.
   * TODO In passing, one may probably avoid keeping the Model::Type of each
   * series in two places:
   *  1) call initCursorParameters elsewhere the very first time the graph view
   *     appears,
   *  2) take into account the Model::Type in the computation of the
   *     storeChecksum in order to detect any change in the series and in
   *     their model types. */
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_modelType[i] = m_store->seriesRegressionType(i);
  }

  /* Both the GraphController and the Store hold the Model::Type of each
   * series. The values differ in two cases:
   *  1) the very first time the graph view appears
   *  2) when the user selects another Model::Type for a series.
   * where we decide to place the cursor at a default position. */
  if (previousSelectedRegressionType != m_store->seriesRegressionType(*m_selectedSeriesIndex)) {
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

Poincare::Context * GraphController::globalContext() const {
  return AppsContainer::sharedAppsContainer()->globalContext();
}

void GraphController::computeXRange(float xMinLimit, float xMaxLimit, float * xMin, float * xMax, float * yMinIntrinsic, float * yMaxIntrinsic) {
  *xMin = FLT_MAX;
  *xMax = -FLT_MAX;
  for (int series = 0; series < Store::k_numberOfSeries; series++) {
    if (m_store->seriesIsValid(series)) {
      Poincare::Zoom::CombineRanges(m_store->minValueOfColumn(series, 0), m_store->maxValueOfColumn(series, 0), *xMin, *xMax, xMin, xMax);
    }
  }
  Poincare::Zoom::SanitizeRangeForDisplay(xMin, xMax);
  float dx = (*xMax - *xMin) * k_displayHorizontalMarginRatio;
  *xMin -= dx;
  *xMax += dx;
  *xMin = std::max(xMinLimit, *xMin);
  *xMax = std::min(xMaxLimit, *xMax);
  *yMinIntrinsic = FLT_MAX;
  *yMaxIntrinsic = -FLT_MAX;
}

void GraphController::computeYRange(float xMin, float xMax, float yMinIntrinsic, float yMaxIntrinsic, float * yMin, float * yMax, bool optimizeRange) {
  *yMin = yMinIntrinsic;
  *yMax = yMaxIntrinsic;
  for (int series = 0; series < Store::k_numberOfSeries; series++) {
    for (int pair = 0; pair < m_store->numberOfPairsOfSeries(series); pair++) {
      float x = m_store->get(series, 0, pair);
      if (x < xMin || x > xMax) {
        continue;
      }
      float y = m_store->get(series, 1, pair);
      Poincare::Zoom::CombineRanges(*yMin, *yMax, y, y, yMin, yMax);
    }
  }
  Poincare::Zoom::SanitizeRangeForDisplay(yMin, yMax, Store::NormalYXRatio() * (xMax - xMin) / 2.f);
}

// Private

KDCoordinate GraphController::SeriesSelectionController::rowHeight(int j) {
  if (j < 0 || j >= numberOfRows()) {
    return 0;
  }
  return KDFont::LargeFont->glyphSize().height() + Metric::CellTopMargin + Metric::CellBottomMargin;
}

void GraphController::SeriesSelectionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  char name[] = "X?/Y?";
  int j = graphController()->m_store->indexOfKthValidSeries(index);
  name[1] = name[4] = '1' + j;
  static_cast<CurveSelectionCell *>(cell)->setLayout(LayoutHelper::String(name, sizeof(name) / sizeof(char)));
}

bool GraphController::buildRegressionExpression(char * buffer, size_t bufferSize, Model::Type modelType, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  int length = Poincare::Print::safeCustomPrintf(buffer, bufferSize, "%s", "ŷ=");
  if (length >= bufferSize) {
    return false;
  }
  buffer += length;
  bufferSize -= length;

  double * coefficients = m_store->coefficientsForSeries(*m_selectedSeriesIndex, globalContext());
  length = m_store->regressionModel(modelType)->buildEquationTemplate(buffer, bufferSize, coefficients, significantDigits, displayMode);
  return length < bufferSize;
}

// SimpleInteractiveCurveViewController

void GraphController::reloadBannerView() {
  const int significantDigits = Preferences::sharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode = Poincare::Preferences::sharedPreferences()->displayMode();

  // If any coefficient is NAN, display that data is not suitable
  bool coefficientsAreDefined = m_store->coefficientsAreDefined(*m_selectedSeriesIndex, globalContext());
  bool displayMean = (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex));
  char buffer[k_bannerViewTextBufferSize];
  Model::Type modelType = m_store->seriesRegressionType(*m_selectedSeriesIndex);
  if (*m_selectedDotIndex < 0 && coefficientsAreDefined && buildRegressionExpression(buffer, k_bannerViewTextBufferSize, modelType, significantDigits, displayMode)) {
    // Regression equation fits in the banner, display it
    m_bannerView.setDisplayParameters(true, false, false);
    m_bannerView.otherView()->setText(buffer);
  } else if (displayMean) {
    // Display MeanDot before x= and y=
    m_bannerView.setDisplayParameters(true, true, !coefficientsAreDefined);
    m_bannerView.otherView()->setText(I18n::translate(I18n::Message::MeanDot));
  } else {
    // Nothing else to display
    m_bannerView.setDisplayParameters(false, false, !coefficientsAreDefined);
    m_bannerView.otherView()->setText("");
  }

  /* Use "x=..." or "xmean=..." (\xCC\x85 represents the combining overline ' ̅')
   * if the mean dot is selected. Same with y. */
  m_bannerView.abscissaSymbol()->setText(displayMean ? "x\xCC\x85=" : "x=");
  double x = displayMean ? m_store->meanOfColumn(*m_selectedSeriesIndex, 0) : m_cursor->x();
  Poincare::Print::customPrintf(buffer, k_bannerViewTextBufferSize, "%*.*ed", x, displayMode, significantDigits);
  m_bannerView.abscissaValue()->setText(buffer);

  double y = displayMean ? m_store->meanOfColumn(*m_selectedSeriesIndex, 1) : m_cursor->y();
  Poincare::Print::customPrintf(buffer, k_bannerViewTextBufferSize, (displayMean ? "y\xCC\x85=%*.*ed" : "y=%*.*ed"), y, displayMode, significantDigits);
  m_bannerView.ordinateView()->setText(buffer);

  m_bannerView.reload();
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  double x;
  double y;
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(*m_selectedSeriesIndex, direction, *m_selectedDotIndex);
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

CurveView * GraphController::curveView() {
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
    stackController()->push(controller);
  } else {
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

bool GraphController::cursorMatchesModel() {
  if (!m_store->seriesIsValid(*m_selectedSeriesIndex)) {
    return false;
  }
  Coordinate2D<double> xy;
  if (*m_selectedDotIndex == -1) {
    xy = xyValues(*m_selectedSeriesIndex, m_cursor->t(), globalContext());
  } else if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    assert(!selectedSeriesIsScatterPlot());
    xy = Coordinate2D<double>(m_store->meanOfColumn(*m_selectedSeriesIndex, 0), m_store->meanOfColumn(*m_selectedSeriesIndex, 1));
  } else if (*m_selectedDotIndex > m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    return false;
  } else {
    xy = Coordinate2D<double>(m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex), m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex));
  }
  return PoincareHelpers::equalOrBothNan(xy.x1(), m_cursor->x()) && PoincareHelpers::equalOrBothNan(xy.x2(), m_cursor->y());
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
    *m_selectedSeriesIndex = closestRegressionSeries;
    *m_selectedDotIndex = -1;
    setRoundCrossCursorView();
    m_cursor->moveTo(x, x, yValue(*m_selectedSeriesIndex, x, context));
    return true;
  }

  if (validDot) {
    // Select the dot
    *m_selectedSeriesIndex = closestDotSeries;
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
    return true;
  }

  // There was no suitable selection
  return false;
}

uint32_t GraphController::rangeVersion() {
  return m_store->rangeChecksum();
}

int GraphController::selectedCurveRelativePosition() const {
  int res = *m_selectedSeriesIndex;
  if (res < 0) {
    return -1;
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
  if (round) {
    // Set the color although the cursor view stays round
    assert(*m_selectedSeriesIndex < static_cast<int>(Palette::numberOfDataColors()));
    m_roundCursorView.setColor(Palette::DataColor[*m_selectedSeriesIndex]);
  }
  CursorView * nextCursorView = round ? static_cast<Shared::CursorView *>(&m_roundCursorView) : static_cast<Shared::CursorView *>(&m_crossCursorView);
  // Escape if the cursor view stays the same
  if (m_view.cursorView() == nextCursorView) {
    return;
  }
#ifdef GRAPH_CURSOR_SPEEDUP
  m_roundCursorView.resetMemoization();
#endif
  m_view.setCursorView(nextCursorView);
}

}
