#include "graph_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout.h>
#include <poincare/numeric/zoom.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include <algorithm>
#include <cmath>

#include "../app.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

GraphController::GraphController(
    Responder* parentResponder, ButtonRowController* header,
    Shared::InteractiveCurveViewRange* interactiveRange,
    CurveViewCursor* cursor, int* selectedDotIndex, int* selectedCurveIndex,
    Store* store)
    : InteractiveCurveViewController(parentResponder, header, interactiveRange,
                                     &m_view, cursor, I18n::Message::Regression,
                                     selectedCurveIndex),
      m_bannerView(this, this),
      m_view(interactiveRange, store, m_cursor, &m_bannerView, &m_cursorView),
      m_store(store),
      m_graphOptionsController(this, interactiveRange, m_store, m_cursor, this),
      m_curveSelectionController(this),
      m_selectedDotIndex(selectedDotIndex),
      m_selectedModelType((Model::Type)-1) {
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
  Model::Type newSelectedModelType =
      regressionTypeOfCurve(*m_selectedCurveIndex);
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
  m_bannerView.abscissaValue()->setDelegate(this);
  App::app()->setFirstResponder(m_bannerView.abscissaValue());
}

Poincare::Context* GraphController::globalContext() const {
  return AppsContainerHelper::sharedAppsContainerGlobalContext();
}

// Private

KDCoordinate GraphController::CurveSelectionController::nonMemoizedRowHeight(
    int row) {
  if (row < 0 || row >= numberOfRows()) {
    return 0;
  }
  return KDFont::GlyphHeight(KDFont::Size::Large) +
         Metric::CellMargins.height();
}

void GraphController::CurveSelectionController::fillCellForRow(
    HighlightCell* cell, int row) {
  int series = graphController()->seriesIndexFromCurveIndex(row);
  const char* name = Store::SeriesTitle(series);
  CurveSelectionCellWithChevron* myCell =
      static_cast<CurveSelectionCellWithChevron*>(cell);
  myCell->setColor(DoublePairStore::colorOfSeriesAtIndex(series));
  myCell->label()->setLayout(Layout::String(name));
}

bool GraphController::buildRegressionExpression(
    char* buffer, size_t bufferSize, Model::Type modelType,
    int significantDigits,
    Poincare::Preferences::PrintFloatMode displayMode) const {
  double* coefficients =
      m_store->coefficientsForSeries(selectedSeriesIndex(), globalContext());
  Layout l = m_store->regressionModel(modelType)->equationLayout(
      coefficients,
      GlobalPreferences::SharedGlobalPreferences()->yPredictedSymbol(),
      significantDigits, displayMode);
  size_t length = l.serialize(buffer, bufferSize);
  if (length >= bufferSize - 1 || length == 0) {
    return false;
  }
  return SerializationHelper::
             ReplaceSystemParenthesesAndBracesByUserParentheses(
                 buffer, length) < bufferSize - 1;
}

// SimpleInteractiveCurveViewController

void GraphController::reloadBannerView() {
  const int selectedSeries = selectedSeriesIndex();
  const int significantDigits =
      Preferences::SharedPreferences()->numberOfSignificantDigits();
  Poincare::Preferences::PrintFloatMode displayMode =
      Poincare::Preferences::SharedPreferences()->displayMode();

  // If any coefficient is NAN, display that data is not suitable
  bool coefficientsAreDefined =
      m_store->coefficientsAreDefined(selectedSeries, globalContext());
  if (coefficientsAreDefined && *m_selectedDotIndex < 0 &&
      curveIsScatterPlot(*m_selectedCurveIndex)) {
    // Regression model has been removed, reinitialize cursor
    initCursorParameters();
  }
  bool displayMean =
      (*m_selectedDotIndex == numberOfDotsOfCurve(*m_selectedCurveIndex));
  bool displayEquation = (*m_selectedDotIndex < 0);
  char buffer[BannerView::BannerBufferTextView::MaxTextSize()];
  Model::Type modelType = regressionTypeOfCurve(*m_selectedCurveIndex);
  if (displayEquation && coefficientsAreDefined &&
      buildRegressionExpression(buffer,
                                BannerView::BannerBufferTextView::MaxTextSize(),
                                modelType, significantDigits, displayMode)) {
    // Regression equation fits in the banner, display it
    m_bannerView.setDisplayParameters(true, false, false);
    m_bannerView.otherView()->setText(buffer);
  } else if (displayMean) {
    // Display MeanDot before x= and y=
    m_bannerView.setDisplayParameters(true, true, !coefficientsAreDefined);
    m_bannerView.otherView()->setText(I18n::translate(I18n::Message::MeanDot));
  } else if (!(Store::HasCoefficients(modelType) ||
               Preferences::SharedPreferences()
                   ->examMode()
                   .forbidStatsDiagnostics())) {
    // Display correlation coefficient
    m_bannerView.setDisplayParameters(true, false, !coefficientsAreDefined);
    Poincare::Print::CustomPrintf(
        buffer, BannerView::BannerBufferTextView::MaxTextSize(), "r=%*.*ed",
        m_store->correlationCoefficient(selectedSeries), displayMode,
        significantDigits);
    m_bannerView.otherView()->setText(buffer);
  } else {
    // Nothing else to display
    m_bannerView.setDisplayParameters(false, false, !coefficientsAreDefined);
    m_bannerView.otherView()->setText("");
  }

  /* Use "x=..." or "xmean=..." (\xCC\x85 represents the combining overline ' ̅')
   * if the mean dot is selected. Same with y. */
  double x =
      displayMean ? m_store->meanOfColumn(selectedSeries, 0) : m_cursor->x();
  Poincare::Print::CustomPrintf(
      buffer, BannerView::BannerBufferTextView::MaxTextSize() - 3, "%*.*ed", x,
      displayMode, significantDigits);  // -3 for "x\xCC\x85="
  m_bannerView.abscissaValue()->setText(buffer);
  m_bannerView.abscissaSymbol()->setText(displayMean ? "x\xCC\x85=" : "x=");

  double y =
      displayMean ? m_store->meanOfColumn(selectedSeries, 1) : m_cursor->y();
  Poincare::Print::CustomPrintf(
      buffer, BannerView::BannerBufferTextView::MaxTextSize(), "%s=%*.*ed",
      (displayMean
           ? "y\xCC\x85"
           : (displayEquation ? GlobalPreferences::SharedGlobalPreferences()
                                    ->yPredictedSymbol()
                              : "y")),
      y, displayMode, significantDigits);
  m_bannerView.ordinateView()->setText(buffer);

  m_bannerView.reload();
}

bool GraphController::moveCursorHorizontally(OMG::HorizontalDirection direction,
                                             int scrollSpeed) {
  double x;
  double y;
  if (*m_selectedDotIndex >= 0) {
    int dotSelected =
        m_store->nextDot(selectedSeriesIndex(), direction, *m_selectedDotIndex,
                         !curveIsScatterPlot(*m_selectedCurveIndex));
    if (dotSelected >= 0) {
      x = dotAbscissa(*m_selectedCurveIndex, dotSelected);
      y = dotOrdinate(*m_selectedCurveIndex, dotSelected);
    } else {
      return false;
    }
    *m_selectedDotIndex = dotSelected;
  } else {
    double step =
        (direction.isRight() ? 1 : -1) * scrollSpeed *
        static_cast<double>(interactiveCurveViewRange()->xGridUnit()) /
        static_cast<double>(k_numberOfCursorStepsInGradUnit);
    x = m_cursor->x() + step;
    y = yValue(*m_selectedCurveIndex, x, globalContext());
  }
  m_cursor->moveTo(x, x, y);
  return true;
}

InteractiveCurveViewRange* GraphController::interactiveCurveViewRange() const {
  return App::app()->graphRange();
}

void GraphController::openMenuForCurveAtIndex(int curveIndex) {
  if (*m_selectedCurveIndex != curveIndex) {
    *m_selectedCurveIndex = curveIndex;
    Coordinate2D<double> xy =
        xyValues(curveIndex, m_cursor->t(), App::app()->localContext());
    m_cursor->moveTo(m_cursor->t(), xy.x(), xy.y());
  }
  if (curveIsScatterPlot(*m_selectedCurveIndex)) {
    // Push regression controller directly
    RegressionController* controller = App::app()->regressionController();
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
  if (curveIsScatterPlot(*m_selectedCurveIndex)) {
    *m_selectedDotIndex = 0;
  } else {
    *m_selectedDotIndex = numberOfDotsOfCurve(*m_selectedCurveIndex);
  }
  double x = dotAbscissa(*m_selectedCurveIndex, *m_selectedDotIndex);
  double y = dotOrdinate(*m_selectedCurveIndex, *m_selectedDotIndex);
  m_cursor->moveTo(x, x, y);
}

bool GraphController::selectedModelIsValid() const {
  const uint8_t numberOfDots = numberOfDotsOfCurve(*m_selectedCurveIndex);
  return *m_selectedDotIndex < numberOfDots ||
         (*m_selectedDotIndex == numberOfDots &&
          !curveIsScatterPlot(*m_selectedCurveIndex));
}

Poincare::Coordinate2D<double> GraphController::selectedModelXyValues(
    double t) const {
  assert(selectedModelIsValid());
  if (*m_selectedDotIndex == -1) {
    return xyValues(*m_selectedCurveIndex, t, globalContext());
  }
  return Coordinate2D<double>(
      dotAbscissa(*m_selectedCurveIndex, *m_selectedDotIndex),
      dotOrdinate(*m_selectedCurveIndex, *m_selectedDotIndex));
}

bool GraphController::moveCursorVertically(OMG::VerticalDirection direction) {
  Poincare::Context* context = globalContext();
  double x = m_cursor->x();
  double y = m_cursor->y();

  // Find the closest regression
  int selectedRegressionCurve =
      *m_selectedDotIndex == -1 ? *m_selectedCurveIndex : -1;
  int closestRegressionCurve =
      closestCurveIndexVertically(direction, selectedRegressionCurve, context);

  // Find the closest dot
  int closesDotSeries = -1;
  int dotSelected = m_store->closestVerticalDot(
      direction, x, y, selectedSeriesIndex(), *m_selectedDotIndex,
      &closesDotSeries, context);
  int closesDotCurve =
      closesDotSeries == -1 ? -1 : curveIndexFromSeriesIndex(closesDotSeries);

  // Choose between selecting the regression or the dot
  bool validRegression = closestRegressionCurve >= 0;
  bool validDot =
      0 <= dotSelected && dotSelected <= numberOfDotsOfCurve(closesDotCurve);
  if (validRegression && validDot) {
    /* Compare the abscissa distances to select either the dot or the
     * regression. If they are equal, compare the ordinate distances. */
    double dotDistanceX =
        std::fabs(dotAbscissa(closesDotCurve, dotSelected) - x);
    if (dotDistanceX != 0) {
      /* The regression X distance to the point is 0, so it is closer than the
       * dot. */
      validDot = false;
    } else {
      // Compare the y distances
      double regressionDistanceY =
          std::fabs(yValue(closestRegressionCurve, x, context) - y);
      double dotDistanceY =
          std::fabs(dotOrdinate(closesDotCurve, dotSelected) - y);
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
    if (*m_selectedCurveIndex != closestRegressionCurve) {
      *m_selectedCurveIndex = closestRegressionCurve;
      // Reload so that the selected series is on top
      m_view.reload(false, true);
    }
    *m_selectedDotIndex = -1;
    setRoundCrossCursorView();
    double newY = yValue(*m_selectedCurveIndex, x, context);
    m_cursor->moveTo(x, x, newY);
    setAbscissaInputAsFirstResponder();
    return true;
  }

  if (validDot) {
    // Select the dot
    if (*m_selectedCurveIndex != closesDotCurve) {
      *m_selectedCurveIndex = closesDotCurve;
      // Reload so that the selected curve is on top
      m_view.reload(false, true);
    }
    *m_selectedDotIndex = dotSelected;
    setRoundCrossCursorView();
    double newX = dotAbscissa(*m_selectedCurveIndex, dotSelected);
    double newY = dotOrdinate(*m_selectedCurveIndex, dotSelected);
    m_cursor->moveTo(newX, newX, newY);
    // abscissa input must resolve first responder
    App::app()->setFirstResponder(this);
    return true;
  }

  // There was no suitable selection
  return false;
}

Coordinate2D<double> GraphController::xyValues(int curveIndex, double t,
                                               Poincare::Context* context,
                                               int subCurveIndex) const {
  return Coordinate2D<double>(t, yValue(curveIndex, t, context));
}

bool GraphController::suitableYValue(double y) const {
  return interactiveCurveViewRange()->yMin() <= y &&
         y <= interactiveCurveViewRange()->yMax();
}

void GraphController::setRoundCrossCursorView() {
  /* At this point, the model (selected series and dot indices) should be up
   * to date. */
  bool round = *m_selectedDotIndex < 0;
  m_cursorView.setIsRing(!round);
  assert(selectedSeriesIndex() <
         static_cast<int>(Palette::numberOfDataColors()));
  m_cursorView.setColor(Palette::DataColor[selectedSeriesIndex()], &m_view);
}

Range2D<float> GraphController::optimalRange(
    bool computeX, bool computeY, Range2D<float> originalRange) const {
  Range1D<float> xRange, yRange;
  const int nbOfCurves = numberOfCurves();
  for (int curve = 0; curve < nbOfCurves; curve++) {
    const int numberOfDots = numberOfDotsOfCurve(curve);
    for (int dot = 0; dot < numberOfDots; dot++) {
      float x = dotAbscissa(curve, dot);
      xRange.extend(x, k_maxFloat);
      if (computeX ||
          (originalRange.xMin() <= x && x <= originalRange.xMax())) {
        float y = dotOrdinate(curve, dot);
        yRange.extend(y, k_maxFloat);
      }
    }
  }
  Range2D<float> result(computeX ? xRange : *originalRange.x(),
                        computeY ? yRange : *originalRange.y());
  return Zoom::Sanitize(result, InteractiveCurveViewRange::NormalYXRatio(),
                        k_maxFloat);
}

double GraphController::dotCoordinate(int curveIndex, int dotIndex,
                                      int coordinate) const {
  assert(dotIndex != numberOfDotsOfCurve(curveIndex) ||
         !curveIsScatterPlot(curveIndex));
  assert(0 <= dotIndex && dotIndex <= numberOfDotsOfCurve(curveIndex));
  const int seriesIndex = seriesIndexFromCurveIndex(curveIndex);
  return dotIndex < m_store->numberOfPairsOfSeries(seriesIndex)
             ? m_store->get(seriesIndex, coordinate, dotIndex)
             : m_store->meanOfColumn(seriesIndex, coordinate);
}

}  // namespace Regression
