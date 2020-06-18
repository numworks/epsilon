#include "graph_controller.h"
#include "../shared/poincare_helpers.h"
#include "../shared/text_helpers.h"
#include "../apps_container.h"
#include <poincare/preferences.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;
using namespace Shared;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Store * store, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion, int * selectedDotIndex, int * selectedSeriesIndex) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, store, &m_view, cursor, modelVersion, previousModelsVersions, rangeVersion),
  m_crossCursorView(),
  m_roundCursorView(),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(store, m_cursor, &m_bannerView, &m_crossCursorView),
  m_store(store),
  m_initialisationParameterController(this, m_store),
  m_graphOptionsController(this, inputEventHandlerDelegate, m_store, m_cursor, this),
  m_selectedDotIndex(selectedDotIndex),
  m_selectedSeriesIndex(selectedSeriesIndex)
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_modelType[i] = (Model::Type) -1;
  }
  m_store->setDelegate(this);
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

bool GraphController::isEmpty() const {
  return m_store->isEmpty();
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
  if (*m_selectedSeriesIndex < 0 || m_store->seriesIsEmpty(*m_selectedSeriesIndex)) {
    *m_selectedSeriesIndex = m_store->indexOfKthNonEmptySeries(0);
  }

  /* Both the GraphController and the Store hold the Model::Type of each
   * series. The values differ in two cases:
   *  1) the very first time the graph view appears
   *  2) when the user selects another Model::Type for a series.
   * where we decide to place the cursor at a default position. */
  if (m_modelType[*m_selectedSeriesIndex] != m_store->seriesRegressionType(*m_selectedSeriesIndex)) {
    initCursorParameters();
  }

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
    m_modelType[i] = m_store->seriesRegressionType(*m_selectedSeriesIndex);
  }

  /* The following
   *   - calls initCursorParameters() if necessary,
   *   - reloads the bannerView and the curveView. */
  InteractiveCurveViewController::viewWillAppear();

  /* Since *m_selectedDotIndex is altered by initCursorParameters(),
   * the following must absolutely come at the end. */
  setRoundCrossCursorView();
}

// Private

Poincare::Context * GraphController::globalContext() {
  return AppsContainer::sharedAppsContainer()->globalContext();
}

// SimpleInteractiveCurveViewController

void GraphController::reloadBannerView() {
  // Set point equals: "P(...) ="
  constexpr size_t bufferSize = k_maxNumberOfCharacters + PrintFloat::charSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  int numberOfChar = 0;
  const char * legend = " P(";
  numberOfChar += strlcpy(buffer, legend, bufferSize);
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    legend = I18n::translate(I18n::Message::MeanDot);
    assert(numberOfChar <= bufferSize);
    numberOfChar += strlcpy(buffer + numberOfChar, legend, bufferSize - numberOfChar);
  } else if (*m_selectedDotIndex < 0) {
    legend = I18n::translate(I18n::Message::Reg);
    assert(numberOfChar <= bufferSize);
    numberOfChar += strlcpy(buffer + numberOfChar, legend, bufferSize - numberOfChar);
  } else {
    numberOfChar += PoincareHelpers::ConvertFloatToTextWithDisplayMode<float>(std::round((float)*m_selectedDotIndex+1.0f), buffer + numberOfChar, bufferSize - numberOfChar, Preferences::ShortNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
  }
  legend = ")  ";
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer + numberOfChar, legend, bufferSize - numberOfChar);
  m_bannerView.dotNameView()->setText(buffer);

  // Set "x=..." or "xmean=..."
  legend = "x=";
  double x = m_cursor->x();
  // Display a specific legend if the mean dot is selected
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    // \xCC\x85 represents the combining overline ' ̅'
    legend = "x\xCC\x85=";
    x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
  }
  m_bannerView.abscissaSymbol()->setText(legend);

  numberOfChar = PoincareHelpers::ConvertFloatToText<double>(x, buffer, bufferSize, Preferences::MediumNumberOfSignificantDigits);
  // Padding
  Shared::TextHelpers::PadWithSpaces(buffer, bufferSize, &numberOfChar, k_maxLegendLength);
  m_bannerView.abscissaValue()->setText(buffer);

  // Set "y=..." or "ymean=..."
  numberOfChar = 0;
  legend = "y=";
  double y = m_cursor->y();
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    // \xCC\x85 represents the combining overline ' ̅'
    legend = "y\xCC\x85=";
    y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
  }
  numberOfChar += strlcpy(buffer, legend, bufferSize);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer + numberOfChar, bufferSize - numberOfChar, Preferences::MediumNumberOfSignificantDigits);
  // Padding
  Shared::TextHelpers::PadWithSpaces(buffer, bufferSize, &numberOfChar, k_maxLegendLength);
  m_bannerView.ordinateView()->setText(buffer);

  // Set formula
  Model * model = m_store->modelForSeries(*m_selectedSeriesIndex);
  I18n::Message formula = model->formulaMessage();
  m_bannerView.regressionTypeView()->setMessage(formula);

  // Get the coefficients
  double * coefficients = m_store->coefficientsForSeries(*m_selectedSeriesIndex, globalContext());
  bool coefficientsAreDefined = true;
  for (int i = 0; i < model->numberOfCoefficients(); i++) {
    if (std::isnan(coefficients[i])) {
      coefficientsAreDefined = false;
      break;
    }
  }
  if (!coefficientsAreDefined) {
    // Force the "Data not suitable" message to be on the next line
    int numberOfCharToCompleteLine = std::max<int>(Ion::Display::Width / BannerView::Font()->glyphSize().width() - strlen(I18n::translate(formula)), 0);
    numberOfChar = 0;
    // Padding
    Shared::TextHelpers::PadWithSpaces(buffer, bufferSize, &numberOfChar, numberOfCharToCompleteLine - 1);
    m_bannerView.subTextAtIndex(0)->setText(buffer);

    const char * dataNotSuitableMessage = I18n::translate(I18n::Message::DataNotSuitableForRegression);
    m_bannerView.subTextAtIndex(1)->setText(const_cast<char *>(dataNotSuitableMessage));
    for (int i = 2; i < m_bannerView.numberOfsubTexts(); i++) {
      m_bannerView.subTextAtIndex(i)->setText("");
    }
    m_bannerView.reload();
    return;
  }
  char coefficientName = 'a';
  for (int i = 0; i < model->numberOfCoefficients(); i++) {
    numberOfChar = 0;
    char leg[] = {' ', coefficientName, '=', 0};
    legend = leg;
    numberOfChar += strlcpy(buffer, legend, bufferSize);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(coefficients[i], buffer + numberOfChar, bufferSize - numberOfChar, Preferences::LargeNumberOfSignificantDigits);
    m_bannerView.subTextAtIndex(i)->setText(buffer);
    coefficientName++;
  }

  if (m_store->seriesRegressionType(*m_selectedSeriesIndex) == Model::Type::Linear) {
    int index = model->numberOfCoefficients();
    // Set "r=..."
    numberOfChar = 0;
    legend = " r=";
    double r = m_store->correlationCoefficient(*m_selectedSeriesIndex);
    numberOfChar += strlcpy(buffer, legend, bufferSize);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(r, buffer + numberOfChar, bufferSize - numberOfChar, Preferences::LargeNumberOfSignificantDigits);
    m_bannerView.subTextAtIndex(0+index)->setText(buffer);

    // Set "r2=..."
    numberOfChar = 0;
    legend = " r2=";
    double r2 = m_store->squaredCorrelationCoefficient(*m_selectedSeriesIndex);
    numberOfChar += strlcpy(buffer, legend, bufferSize);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(r2, buffer + numberOfChar, bufferSize - numberOfChar, Preferences::LargeNumberOfSignificantDigits);
    m_bannerView.subTextAtIndex(1+index)->setText(buffer);

    // Clean the last subview
    buffer[0] = 0;
    m_bannerView.subTextAtIndex(2+index)->setText(buffer);

  } else {
    // Empty all non used subviews
    for (int i = model->numberOfCoefficients(); i < m_bannerView.numberOfsubTexts(); i++) {
      buffer[0] = 0;
      m_bannerView.subTextAtIndex(i)->setText(buffer);
    }
  }
  m_bannerView.reload();
}

bool GraphController::moveCursorHorizontally(int direction, bool fast) {
  double x;
  double y;
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(*m_selectedSeriesIndex, direction, *m_selectedDotIndex);
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      x = m_store->get(*m_selectedSeriesIndex, 0, dotSelected);
      y = m_store->get(*m_selectedSeriesIndex, 1, dotSelected);
    } else if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
      y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
    } else {
      return false;
    }
    *m_selectedDotIndex = dotSelected;
  } else {
    x = m_cursor->x() + direction * m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit;
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

bool GraphController::handleEnter() {
  stackController()->push(&m_graphOptionsController);
  return true;
}

// InteractiveCurveViewController
void GraphController::initCursorParameters() {
  double x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
  double y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
  m_cursor->moveTo(x, x, y);
  if (m_store->yAuto()) {
    m_store->panToMakePointVisible(x, y, cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  }
  *m_selectedDotIndex = m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex);
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

uint32_t GraphController::modelVersion() {
  return m_store->storeChecksum();
}

uint32_t GraphController::modelVersionAtIndex(int i) {
  assert(i < numberOfMemoizedVersions());
  return *(m_store->seriesChecksum() + i);
}

uint32_t GraphController::rangeVersion() {
  return m_store->rangeChecksum();
}

bool GraphController::closestCurveIndexIsSuitable(int newIndex, int currentIndex) const {
  return newIndex != currentIndex && !m_store->seriesIsEmpty(newIndex);
}

Coordinate2D<double> GraphController::xyValues(int curveIndex, double x, Poincare::Context * context) const {
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

int GraphController::estimatedBannerNumberOfLines() const {
  return (selectedSeriesIndex() < 0) ? 3 : m_store->modelForSeries(selectedSeriesIndex())->bannerLinesCount();
}

InteractiveCurveViewRangeDelegate::Range GraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  float minY = FLT_MAX;
  float maxY = -FLT_MAX;
  for (int series = 0; series < Store::k_numberOfSeries; series++) {
    for (int k = 0; k < m_store->numberOfPairsOfSeries(series); k++) {
      if (m_store->xMin() <= m_store->get(series, 0, k) && m_store->get(series, 0, k) <= m_store->xMax()) {
        minY = std::min<float>(minY, m_store->get(series, 1, k));
        maxY = std::max<float>(maxY, m_store->get(series, 1, k));
      }
    }
  }
  InteractiveCurveViewRangeDelegate::Range range;
  range.min = minY;
  range.max = maxY;
  return range;
}

void GraphController::setRoundCrossCursorView() {
  /* At this point, the model (selected series and dot indices) should be up
   * to date. */
  bool round = *m_selectedDotIndex < 0;
  if (round) {
    // Set the color although the cursor view stays round
    assert(*m_selectedSeriesIndex < Palette::numberOfDataColors());
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
