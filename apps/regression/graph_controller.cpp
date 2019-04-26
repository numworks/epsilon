#include "graph_controller.h"
#include "../shared/poincare_helpers.h"
#include "../apps_container.h"
#include <kandinsky/font.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

static inline float minFloat(float x, float y) { return x < y ? x : y; }
static inline float maxFloat(float x, float y) { return x > y ? x : y; }
static inline int maxInt(int x, int y) { return x > y ? x : y; }

namespace Regression {

GraphController::GraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Store * store, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, int * selectedDotIndex, int * selectedSeriesIndex) :
  InteractiveCurveViewController(parentResponder, inputEventHandlerDelegate, header, store, &m_view, cursor, modelVersion, rangeVersion),
  m_crossCursorView(),
  m_roundCursorView(),
  m_bannerView(),
  m_view(store, m_cursor, &m_bannerView, &m_crossCursorView, this),
  m_store(store),
  m_initialisationParameterController(this, m_store),
  m_graphOptionsController(this, inputEventHandlerDelegate, m_store, m_cursor, this),
  m_selectedDotIndex(selectedDotIndex),
  m_selectedSeriesIndex(selectedSeriesIndex)
{
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_modelType[i] = (Model::Type) -1;
  }
  m_store->setCursor(m_cursor);
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
  InteractiveCurveViewController::viewWillAppear();
  if (m_modelType[*m_selectedSeriesIndex] != m_store->seriesRegressionType(*m_selectedSeriesIndex)) {
    initCursorParameters();
  }
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    m_modelType[i] = m_store->seriesRegressionType(*m_selectedSeriesIndex);
  }
  if (*m_selectedSeriesIndex < 0) {
    *m_selectedSeriesIndex = m_store->indexOfKthNonEmptySeries(0);
  }
  if (*m_selectedDotIndex >= 0) {
    m_view.setCursorView(static_cast<View *>(&m_crossCursorView));
  } else {
    m_view.setCursorView(static_cast<View *>(&m_roundCursorView));
    m_roundCursorView.setColor(Palette::DataColor[*m_selectedSeriesIndex]);
  }
}

void GraphController::selectRegressionCurve() {
  *m_selectedDotIndex = -1;
  m_view.setCursorView(&m_roundCursorView);
  m_roundCursorView.setColor(Palette::DataColor[*m_selectedSeriesIndex]);
}

// Private

Poincare::Context * GraphController::globalContext() {
  return const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext();
}

// SimpleInteractiveCurveViewController

void GraphController::reloadBannerView() {
  if (*m_selectedSeriesIndex < 0) {
    return;
  }

  // Set point equals: "P(...) ="
  constexpr size_t bufferSize = k_maxNumberOfCharacters + PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  int numberOfChar = 0;
  const char * legend = " P(";
  numberOfChar += strlcpy(buffer, legend, bufferSize);
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    legend = I18n::translate(I18n::Message::MeanDot);
    numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize - numberOfChar);
  } else if (*m_selectedDotIndex < 0) {
    legend = I18n::translate(I18n::Message::Reg);
    numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize - numberOfChar);
  } else {
    numberOfChar += PrintFloat::convertFloatToText<float>(std::round((float)*m_selectedDotIndex+1.0f), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits, Preferences::PrintFloatMode::Decimal);
  }
  legend = ")  ";
  strlcpy(buffer+numberOfChar, legend, bufferSize - numberOfChar);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.dotNameView()->setText(buffer);

  // Set "x=..." or "xmean=..."
  numberOfChar = 0;
  legend = "x=";
  double x = m_cursor->x();
  // Display a specific legend if the mean dot is selected
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    // \xCC\x85 represents the combining overline ' ̅'
    legend = "x\xCC\x85=";
    x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
  }
  numberOfChar += strlcpy(buffer, legend, bufferSize);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(x, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  assert(UTF8Decoder::CharSizeOfCodePoint(' ') == 1);
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_bannerView.abscissaView()->setText(buffer);

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
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_bannerView.ordinateView()->setText(buffer);

  // Set formula
  Model * model = m_store->modelForSeries(selectedSeriesIndex());
  I18n::Message formula = model->formulaMessage();
  m_bannerView.regressionTypeView()->setMessage(formula);

  // Get the coefficients
  double * coefficients = m_store->coefficientsForSeries(selectedSeriesIndex(), globalContext());
  bool coefficientsAreDefined = true;
  for (int i = 0; i < model->numberOfCoefficients(); i++) {
    if (std::isnan(coefficients[i])) {
      coefficientsAreDefined = false;
      break;
    }
  }
  if (!coefficientsAreDefined) {
    // Force the "Data not suitable" message to be on the next line
    int numberOfCharToCompleteLine = maxInt(Ion::Display::Width/(m_bannerView.font()->glyphSize().width())- strlen(I18n::translate(formula)), 0);
    numberOfChar = 0;
    for (int i = 0; i < numberOfCharToCompleteLine-1; i++) {
      buffer[numberOfChar++] = ' ';
    }
    buffer[numberOfChar] = 0;
    m_bannerView.subTextAtIndex(0)->setText(buffer);

    const char * dataNotSuitableMessage = I18n::translate(I18n::Message::DataNotSuitableForRegression);
    m_bannerView.subTextAtIndex(1)->setText(const_cast<char *>(dataNotSuitableMessage));
    for (int i = 2; i < m_bannerView.numberOfsubTexts(); i++) {
      m_bannerView.subTextAtIndex(i)->setText("");
    }
    return;
  }
  char coefficientName = 'a';
  for (int i = 0; i < model->numberOfCoefficients(); i++) {
    numberOfChar = 0;
    char leg[] = {' ', coefficientName, '=', 0};
    legend = leg;
    numberOfChar += strlcpy(buffer, legend, bufferSize);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(coefficients[i], buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    buffer[k_maxLegendLength] = 0;
    m_bannerView.subTextAtIndex(i)->setText(buffer);
    coefficientName++;
  }

  if (m_store->seriesRegressionType(selectedSeriesIndex()) == Model::Type::Linear) {
    // Set "r=..."
    numberOfChar = 0;
    legend = " r=";
    double r = m_store->correlationCoefficient(*m_selectedSeriesIndex);
    numberOfChar += strlcpy(buffer, legend, bufferSize);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(r, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    buffer[k_maxLegendLength+10] = 0;
    m_bannerView.subTextAtIndex(2)->setText(buffer);

    // Set "r2=..."
    numberOfChar = 0;
    legend = " r2=";
    double r2 = m_store->squaredCorrelationCoefficient(*m_selectedSeriesIndex);
    numberOfChar += strlcpy(buffer, legend, bufferSize);
    numberOfChar += PoincareHelpers::ConvertFloatToText<double>(r2, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    buffer[k_maxLegendLength] = 0;
    m_bannerView.subTextAtIndex(3)->setText(buffer);

    // Clean the last subview
    buffer[0] = 0;
    m_bannerView.subTextAtIndex(4)->setText(buffer);

  } else {
    // Empty all non used subviews
    for (int i = model->numberOfCoefficients(); i < m_bannerView.numberOfsubTexts(); i++) {
      buffer[0] = 0;
      m_bannerView.subTextAtIndex(i)->setText(buffer);
    }
  }
  m_bannerView.reload();
}

bool GraphController::moveCursorHorizontally(int direction) {
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(*m_selectedSeriesIndex, direction, *m_selectedDotIndex);
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex), m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
      return true;
    }
    if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->meanOfColumn(*m_selectedSeriesIndex, 0), m_store->meanOfColumn(*m_selectedSeriesIndex, 1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
      return true;
    }
    return false;
  }
  double x = direction > 0 ? m_cursor->x() + m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit :
    m_cursor->x() - m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit;
  double y = yValue(*m_selectedSeriesIndex, x, globalContext());
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
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
  if (*m_selectedSeriesIndex < 0 || m_store->seriesIsEmpty(*m_selectedSeriesIndex)) {
    *m_selectedSeriesIndex = m_store->indexOfKthNonEmptySeries(0);
  }
  double x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
  double y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
  m_cursor->moveTo(x, y);
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
  int closestRegressionSeries = InteractiveCurveViewController::closestCurveIndexVertically(direction > 0, selectedRegressionIndex, context);

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

  if (validRegression) {
    // Select the regression
    *m_selectedSeriesIndex = closestRegressionSeries;
    selectRegressionCurve();
    m_cursor->moveTo(x, yValue(*m_selectedSeriesIndex, x, context));
    m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
    return true;
  }

  if (validDot) {
    // Select the dot
    m_view.setCursorView(&m_crossCursorView);
    *m_selectedSeriesIndex = closestDotSeries;
    *m_selectedDotIndex = dotSelected;
    if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      // Select the mean dot
      m_cursor->moveTo(m_store->meanOfColumn(*m_selectedSeriesIndex, 0), m_store->meanOfColumn(*m_selectedSeriesIndex, 1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
    } else {
      // Select a data point dot
      m_cursor->moveTo(m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex), m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
    }
    return true;
  }

  // There was no suitable selection
  return false;
}

uint32_t GraphController::modelVersion() {
  return m_store->storeChecksum();
}

uint32_t GraphController::rangeVersion() {
  return m_store->rangeChecksum();
}

bool GraphController::isCursorVisible() {
  return interactiveCurveViewRange()->isCursorVisible(cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
}

bool GraphController::closestCurveIndexIsSuitable(int newIndex, int currentIndex) const {
  return newIndex != currentIndex && !m_store->seriesIsEmpty(newIndex);
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

float GraphController::displayTopMarginRatio() {
  return 0.12f; // cursorHeight/graphViewHeight
}

InteractiveCurveViewRangeDelegate::Range GraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  float minY = FLT_MAX;
  float maxY = -FLT_MAX;
  for (int series = 0; series < Store::k_numberOfSeries; series++) {
    for (int k = 0; k < m_store->numberOfPairsOfSeries(series); k++) {
      if (m_store->xMin() <= m_store->get(series, 0, k) && m_store->get(series, 0, k) <= m_store->xMax()) {
        minY = minFloat(minY, m_store->get(series, 1, k));
        maxY = maxFloat(maxY, m_store->get(series, 1, k));
      }
    }
  }
  InteractiveCurveViewRangeDelegate::Range range;
  range.min = minY;
  range.max = maxY;
  return range;
}

}
