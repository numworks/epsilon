#include "continuous_function.h"
#include "poincare_helpers.h"
#include <apps/constant.h>
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/trigonometry.h>
#include <escher/palette.h>
#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <apps/i18n.h>
#include <float.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;

namespace Shared {

void ContinuousFunction::DefaultName(char buffer[], size_t bufferSize) {
  constexpr int k_maxNumberOfDefaultLetterNames = 4;
  static constexpr const char k_defaultLetterNames[k_maxNumberOfDefaultLetterNames] = {
    'f', 'g', 'h', 'p'
  };
  /* First default names are f, g, h, p and then f0, f1... ie, "f[number]",
   * for instance "f12", that does not exist yet in the storage. */
  size_t constantNameLength = 1; // 'f', no null-terminating char
  assert(bufferSize > constantNameLength+1);
  // Find the next available name
  int currentNumber = -k_maxNumberOfDefaultLetterNames;
  int currentNumberLength = 0;
  int availableBufferSize = bufferSize - constantNameLength;
  while (currentNumberLength < availableBufferSize) {
    // Choose letter
    buffer[0] = currentNumber < 0 ? k_defaultLetterNames[k_maxNumberOfDefaultLetterNames+currentNumber] : k_defaultLetterNames[0];
    // Choose number if required
    if (currentNumber >= 0) {
      currentNumberLength = Poincare::Integer(currentNumber).serialize(&buffer[1], availableBufferSize);
    } else {
      buffer[1] = 0;
    }
    if (GlobalContext::SymbolAbstractNameIsFree(buffer)) {
      // Name found
      break;
    }
    currentNumber++;
  }
  assert(currentNumberLength >= 0 && currentNumberLength < availableBufferSize);
}

ContinuousFunction ContinuousFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
  // Create the record
  char nameBuffer[SymbolAbstract::k_maxNameSize];
  RecordDataBuffer data(Palette::nextDataColor(&s_colorIndex));
  if (baseName == nullptr) {
    DefaultName(nameBuffer, SymbolAbstract::k_maxNameSize);
    baseName = nameBuffer;
  }
  *error = Ion::Storage::sharedStorage()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));

  // Return if error
  if (*error != Ion::Storage::Record::ErrorStatus::None) {
    return ContinuousFunction();
  }

  // Return the ContinuousFunction withthe new record
  return ContinuousFunction(Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(baseName, Ion::Storage::funcExtension));
}

int ContinuousFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize) {
  // Fill buffer with f(x). Keep size for derivative sign.
  int derivativeSize = UTF8Decoder::CharSizeOfCodePoint('\'');
  int numberOfChars = nameWithArgument(buffer, bufferSize - derivativeSize);
  assert(numberOfChars + derivativeSize < (int)bufferSize);
  char * firstParenthesis = const_cast<char *>(UTF8Helper::CodePointSearch(buffer, '('));
  if (!UTF8Helper::CodePointIs(firstParenthesis, '(')) {
    return numberOfChars;
  }
  memmove(firstParenthesis + derivativeSize, firstParenthesis, numberOfChars - (firstParenthesis - buffer) + 1);
  UTF8Decoder::CodePointToChars('\'', firstParenthesis, derivativeSize);
  return numberOfChars + derivativeSize;
}

Poincare::Expression ContinuousFunction::expressionReduced(Poincare::Context * context) const {
  Poincare::Expression result = ExpressionModelHandle::expressionReduced(context);
  if (plotType() == PlotType::Parametric && (
      result.type() != Poincare::ExpressionNode::Type::Matrix ||
      static_cast<Poincare::Matrix&>(result).numberOfRows() != 2 ||
      static_cast<Poincare::Matrix&>(result).numberOfColumns() != 1)
     ) {
    return Poincare::Expression::Parse("[[undef][undef]]", nullptr);
  }
  return result;
}

I18n::Message ContinuousFunction::parameterMessageName() const {
  return ParameterMessageForPlotType(plotType());
}

CodePoint ContinuousFunction::symbol() const {
  switch (plotType()) {
  case PlotType::Cartesian:
    return 'x';
  case PlotType::Polar:
    return UCodePointGreekSmallLetterTheta;
  default:
    assert(plotType() == PlotType::Parametric);
    return 't';
  }
}

ContinuousFunction::PlotType ContinuousFunction::plotType() const {
  return recordData()->plotType();
}

void ContinuousFunction::setPlotType(PlotType newPlotType, Poincare::Preferences::AngleUnit angleUnit, Context * context) {
  PlotType currentPlotType = plotType();
  if (newPlotType == currentPlotType) {
    return;
  }

  recordData()->setPlotType(newPlotType);

  setCache(nullptr);

  // Recompute the layouts
  m_model.tidy();

  // Recompute the definition domain
  double tMin = newPlotType == PlotType::Cartesian ? -INFINITY : 0.0;
  double tMax = newPlotType == PlotType::Cartesian ? INFINITY : 2.0*Trigonometry::PiInAngleUnit(angleUnit);
  setTMin(tMin);
  setTMax(tMax);

  /* Recompute the unknowns. For instance, if the function was f(x) = xθ, it is
   * stored as f(?) = ?θ. When switching to polar type, it should be stored as
   * f(?) = ?? */
  constexpr int previousTextContentMaxSize = Constant::MaxSerializedExpressionSize;
  char previousTextContent[previousTextContentMaxSize];
  m_model.text(this, previousTextContent, previousTextContentMaxSize, symbol());
  setContent(previousTextContent, context);

  // Handle parametric function switch
  if (currentPlotType == PlotType::Parametric) {
    Expression e = expressionClone();
    // Change [x(t) y(t)] to y(t)
    if (!e.isUninitialized()
        && e.type() == ExpressionNode::Type::Matrix
        && static_cast<Poincare::Matrix&>(e).numberOfRows() == 2
        && static_cast<Poincare::Matrix&>(e).numberOfColumns() == 1)
    {
      Expression nextContent = e.childAtIndex(1);
      /* We need to detach it, otherwise nextContent will think it has a parent
       * when we retrieve it from the storage. */
      nextContent.detachFromParent();
      setExpressionContent(nextContent);
    }
    return;
  } else if (newPlotType == PlotType::Parametric) {
    Expression e = expressionClone();
    // Change y(t) to [t y(t)]
    Matrix newExpr = Matrix::Builder();
    newExpr.addChildAtIndexInPlace(Symbol::Builder(UCodePointUnknown), 0, 0);
    // if y(t) was not uninitialized, insert [t 2t] to set an example
    e = e.isUninitialized() ? Multiplication::Builder(Rational::Builder(2), Symbol::Builder(UCodePointUnknown)) : e;
    newExpr.addChildAtIndexInPlace(e, newExpr.numberOfChildren(), newExpr.numberOfChildren());
    newExpr.setDimensions(2, 1);
    setExpressionContent(newExpr);
  }
}

I18n::Message ContinuousFunction::ParameterMessageForPlotType(PlotType plotType) {
  if (plotType == PlotType::Cartesian) {
    return I18n::Message::X;
  }
  if (plotType == PlotType::Polar) {
    return I18n::Message::Theta;
  }
  assert(plotType == PlotType::Parametric);
  return I18n::Message::T;
}

template <typename T>
Poincare::Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(T t, Poincare::Context * context) const {
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, context);
  PlotType type = plotType();
  if (type == PlotType::Cartesian || type == PlotType::Parametric) {
    return x1x2;
  }
  assert(type == PlotType::Polar);
  T factor = (T)1.0;
  Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  if (angleUnit == Preferences::AngleUnit::Degree) {
    factor = (T) (M_PI/180.0);
  } else if (angleUnit == Preferences::AngleUnit::Gradian) {
    factor = (T) (M_PI/200.0);
  } else {
    assert(angleUnit == Preferences::AngleUnit::Radian);
  }
  const float angle = x1x2.x1()*factor;
  return Coordinate2D<T>(x1x2.x2() * std::cos(angle), x1x2.x2() * std::sin(angle));
}

bool ContinuousFunction::displayDerivative() const {
  return recordData()->displayDerivative();
}

void ContinuousFunction::setDisplayDerivative(bool display) {
  return recordData()->setDisplayDerivative(display);
}

int ContinuousFunction::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) {
  PlotType type = plotType();
  if (type == PlotType::Cartesian) {
    return Function::printValue(cursorT, cursorX, cursorY, buffer, bufferSize, precision, context);
  }
  if (type == PlotType::Polar) {
    return PoincareHelpers::ConvertFloatToText<double>(evaluate2DAtParameter(cursorT, context).x2(), buffer, bufferSize, precision);
  }
  assert(type == PlotType::Parametric);
  int result = 0;
  result += UTF8Decoder::CodePointToChars('(', buffer+result, bufferSize-result);
  result += PoincareHelpers::ConvertFloatToText<double>(cursorX, buffer+result, bufferSize-result, precision);
  result += UTF8Decoder::CodePointToChars(';', buffer+result, bufferSize-result);
  result += PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer+result, bufferSize-result, precision);
  result += UTF8Decoder::CodePointToChars(')', buffer+result, bufferSize-result);
  return result;
}

double ContinuousFunction::approximateDerivative(double x, Poincare::Context * context) const {
  assert(plotType() == PlotType::Cartesian);
  if (x < tMin() || x > tMax()) {
    return NAN;
  }
  Poincare::Derivative derivative = Poincare::Derivative::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknown), Poincare::Float<double>::Builder(x)); // derivative takes ownership of Poincare::Float<double>::Builder(x) and the clone of expression
  /* TODO: when we approximate derivative, we might want to simplify the
   * derivative here. However, we might want to do it once for all x (to avoid
   * lagging in the derivative table. */
  return PoincareHelpers::ApproximateToScalar<double>(derivative, context);
}

float ContinuousFunction::tMin() const {
  return recordData()->tMin();
}

float ContinuousFunction::tMax() const {
  return recordData()->tMax();
}

void ContinuousFunction::setTMin(float tMin) {
  recordData()->setTMin(tMin);
  setCache(nullptr);
}

void ContinuousFunction::setTMax(float tMax) {
  recordData()->setTMax(tMax);
  setCache(nullptr);
}

void ContinuousFunction::rangeForDisplay(float * xMin, float * xMax, float * yMin, float * yMax, Poincare::Context * context, bool tuneXRange) const {
  if (plotType() == PlotType::Cartesian) {
    interestingXAndYRangesForDisplay(xMin, xMax, yMin, yMax, context, tuneXRange);
  } else {
    fullXYRange(xMin, xMax, yMin, yMax, context);
  }
}

void ContinuousFunction::fullXYRange(float * xMin, float * xMax, float * yMin, float * yMax, Context * context) const {
  assert(yMin && yMax);
  assert(!(std::isinf(tMin()) || std::isinf(tMax()) || std::isnan(rangeStep())));

  float resultXMin = FLT_MAX, resultXMax = - FLT_MAX, resultYMin = FLT_MAX, resultYMax = - FLT_MAX;
  for (float t = tMin(); t <= tMax(); t += rangeStep()) {
    Coordinate2D<float> xy = privateEvaluateXYAtParameter(t, context);
    if (!std::isfinite(xy.x1()) || !std::isfinite(xy.x2())) {
      continue;
    }
    resultXMin = std::min(xy.x1(), resultXMin);
    resultXMax = std::max(xy.x1(), resultXMax);
    resultYMin = std::min(xy.x2(), resultYMin);
    resultYMax = std::max(xy.x2(), resultYMax);
  }
  if (xMin) {
    *xMin = resultXMin;
  }
  if (xMax) {
    *xMax = resultXMax;
  }
  *yMin = resultYMin;
  *yMax = resultYMax;
}

static float evaluateAndRound(const ContinuousFunction * f, float x, Context * context, float precision = 1e-5) {
  /* When evaluating sin(x)/x close to zero using the standard sine function,
   * one can detect small varitions, while the cardinal sine is supposed to be
   * locally monotonous. To smooth our such variations, we round the result of
   * the evaluations. As we are not interested in precise results but only in
   * ordering, this approximation is sufficient. */
  return precision * std::round(f->evaluateXYAtParameter(x, context).x2() / precision);
}


/* TODO : These three methods perform checks that will also be relevant for the
 * equation solver. Remember to factorize this code when integrating the new
 * solver. */
static bool boundOfIntervalOfDefinitionIsReached(float y1, float y2) {
  return std::isfinite(y1) && !std::isinf(y2) && std::isnan(y2);
}
static bool rootExistsOnInterval(float y1, float y2) {
  return ((y1 < 0.f && y2 > 0.f) || (y1 > 0.f && y2 < 0.f));
}
static bool extremumExistsOnInterval(float y1, float y2, float y3) {
  return (y1 < y2 && y2 > y3) || (y1 > y2 && y2 < y3);
}

/* This function checks whether an interval contains an extremum or an
 * asymptote, by recursively computing the slopes. In case of an extremum, the
 * slope should taper off toward the center. */
static bool isExtremum(const ContinuousFunction * f, float x1, float x2, float x3, float y1, float y2, float y3, Context * context, int iterations = 3) {
  if (iterations <= 0) {
    return false;
  }
  float x[2] = {x1, x3}, y[2] = {y1, y3};
  float xm, ym;
  for (int i = 0; i < 2; i++) {
    xm = (x[i] + x2) / 2.f;
    ym = evaluateAndRound(f, xm, context);
    bool res = ((y[i] < ym) != (ym < y2)) ? isExtremum(f, x[i], xm, x2, y[i], ym, y2, context, iterations - 1) : std::fabs(ym - y[i]) >= std::fabs(y2 - ym);
    if (!res) {
      return false;
    }
  }
  return true;
}

enum class PointOfInterest : uint8_t {
  None,
  Bound,
  Extremum,
  Root
};

void ContinuousFunction::interestingXAndYRangesForDisplay(float * xMin, float * xMax, float * yMin, float * yMax, Context * context, bool tuneXRange) const {
  assert(xMin && xMax && yMin && yMax);
  assert(plotType() == PlotType::Cartesian);

  /* Constants of the algorithm. */
  constexpr float defaultMaxInterval = 2e5f;
  constexpr float minDistance = 1e-2f;
  constexpr float asymptoteThreshold = 2e-1f;
  constexpr float stepFactor = 1.1f;
  constexpr int maxNumberOfPoints = 3;
  constexpr float breathingRoom = 0.3f;
  constexpr float maxRatioBetweenPoints = 100.f;

  const bool hasIntervalOfDefinition = std::isfinite(tMin()) && std::isfinite(tMax());
  float center, maxDistance;
  if (!tuneXRange) {
    center = (*xMax + *xMin) / 2.f;
    maxDistance = (*xMax - *xMin) / 2.f;
  } else if (hasIntervalOfDefinition) {
    center = (tMax() + tMin()) / 2.f;
    maxDistance = (tMax() - tMin()) / 2.f;
  } else {
    center = 0.f;
    maxDistance = defaultMaxInterval / 2.f;
  }

  float resultX[2] = {FLT_MAX, - FLT_MAX};
  float resultYMin = FLT_MAX, resultYMax = - FLT_MAX;
  float asymptote[2] = {FLT_MAX, - FLT_MAX};
  int numberOfPoints;
  float xFallback, yFallback[2] = {NAN, NAN};
  float firstResult;
  float dXOld, dXPrev, dXNext, yOld, yPrev, yNext;

  /* Look for an point of interest at the center. */
  const float a = center - minDistance - FLT_EPSILON, b = center + FLT_EPSILON, c = center + minDistance + FLT_EPSILON;
  const float ya = evaluateAndRound(this, a, context), yb = evaluateAndRound(this, b, context), yc = evaluateAndRound(this, c, context);
  if (boundOfIntervalOfDefinitionIsReached(ya, yc) ||
      boundOfIntervalOfDefinitionIsReached(yc, ya) ||
      rootExistsOnInterval(ya, yc) ||
      extremumExistsOnInterval(ya, yb, yc) || ya == yc)
  {
    resultX[0] = resultX[1] = center;
    if (extremumExistsOnInterval(ya, yb, yc) && isExtremum(this, a, b, c, ya, yb, yc, context)) {
      resultYMin = resultYMax = yb;
    }
  }

  /* We search for points of interest by exploring the function leftward from
   * the center and then rightward, hence the two iterations. */
  for (int i = 0; i < 2; i++) {
    /* Initialize the search parameters. */
    numberOfPoints = 0;
    firstResult = NAN;
    xFallback = NAN;
    dXPrev = i == 0 ? - minDistance : minDistance;
    dXNext = dXPrev * stepFactor;
    yPrev = evaluateAndRound(this, center + dXPrev, context);
    yNext = evaluateAndRound(this, center + dXNext, context);

    while(std::fabs(dXPrev) < maxDistance) {
      /* Update the slider. */
      dXOld = dXPrev;
      dXPrev = dXNext;
      dXNext *= stepFactor;
      yOld = yPrev;
      yPrev = yNext;
      yNext = evaluateAndRound(this, center + dXNext, context);
      if (std::isinf(yNext)) {
        continue;
      }
      /* Check for a change in the profile. */
      const PointOfInterest variation = boundOfIntervalOfDefinitionIsReached(yPrev, yNext) ? PointOfInterest::Bound :
                                        rootExistsOnInterval(yPrev, yNext) ? PointOfInterest::Root :
                                        extremumExistsOnInterval(yOld, yPrev, yNext) ? PointOfInterest::Extremum :
                                        PointOfInterest::None;
      switch (static_cast<uint8_t>(variation)) {
        /* The fall through is intentional, as we only want to update the Y
         * range when an extremum is detected, but need to update the X range
         * in all cases. */
        case static_cast<uint8_t>(PointOfInterest::Extremum):
        if (isExtremum(this, center + dXOld, center + dXPrev, center + dXNext, yOld, yPrev, yNext, context)) {
          resultYMin = std::min(resultYMin, yPrev);
          resultYMax = std::max(resultYMax, yPrev);
        }
      case static_cast<uint8_t>(PointOfInterest::Bound):
        /* We only count extrema / discontinuities for limiting the number
         * of points. This prevents cos(x) and cos(x)+2 from having different
         * profiles. */
        if (++numberOfPoints == maxNumberOfPoints) {
          /* When too many points are encountered, we prepare their erasure by
           * setting a restore point. */
          xFallback = dXNext + center;
          yFallback[0] = resultYMin;
          yFallback[1] = resultYMax;
        }
      case static_cast<uint8_t>(PointOfInterest::Root):
        asymptote[i] = i == 0 ? FLT_MAX : - FLT_MAX;
        resultX[0] = std::min(resultX[0], center + (i == 0 ? dXNext : dXPrev));
        resultX[1] = std::max(resultX[1], center + (i == 1 ? dXNext : dXPrev));
        if (std::isnan(firstResult)) {
          firstResult = dXNext;
        }
        break;
      default:
        const float slopeNext = (yNext - yPrev) / (dXNext - dXPrev), slopePrev = (yPrev - yOld) / (dXPrev - dXOld);
        if ((std::fabs(slopeNext) < asymptoteThreshold) && (std::fabs(slopePrev) > asymptoteThreshold)) {
          // Horizontal asymptote begins
          asymptote[i] = (i == 0) ? std::min(asymptote[i], center + dXNext) : std::max(asymptote[i], center + dXNext);
        } else if ((std::fabs(slopeNext) < asymptoteThreshold) && (std::fabs(slopePrev) > asymptoteThreshold)) {
          // Horizontal asymptote invalidates : it might be an asymptote when
          // going the other way.
          asymptote[1 - i] = (i == 1) ? std::min(asymptote[1 - i], center + dXPrev) : std::max(asymptote[1 - i], center + dXPrev);
        }
      }
    }
    if (std::fabs(resultX[i]) > std::fabs(firstResult) * maxRatioBetweenPoints && !std::isnan(xFallback)) {
      /* When there are too many points, cut them if their orders are too
       * different. */
      resultX[i] = xFallback;
      resultYMin = yFallback[0];
      resultYMax = yFallback[1];
    }
  }

  if (tuneXRange) {
    /* Cut after horizontal asymptotes. */
    resultX[0] = std::min(resultX[0], asymptote[0]);
    resultX[1] = std::max(resultX[1], asymptote[1]);
    if (resultX[0] >= resultX[1]) {
      /* Fallback to default range. */
      resultX[0] = - Range1D::k_default;
      resultX[1] = Range1D::k_default;
    } else {
      /* Add breathing room around points of interest. */
      float xRange = resultX[1] - resultX[0];
      resultX[0] -= breathingRoom * xRange;
      resultX[1] += breathingRoom * xRange;
      /* Round to the next integer. */
      resultX[0] = std::floor(resultX[0]);
      resultX[1] = std::ceil(resultX[1]);
    }
    *xMin = std::min(resultX[0], *xMin);
    *xMax = std::max(resultX[1], *xMax);
  }
  *yMin = std::min(resultYMin, *yMin);
  *yMax = std::max(resultYMax, *yMax);

  refinedYRangeForDisplay(*xMin, *xMax, yMin, yMax, context);
}

void ContinuousFunction::refinedYRangeForDisplay(float xMin, float xMax, float * yMin, float * yMax, Context * context) const {
  /* This methods computes the Y range that will be displayed for the cartesian
   * function, given an X range (xMin, xMax) and bounds yMin and yMax that must
   * be inside the Y range.*/
  assert(plotType() == PlotType::Cartesian);
  assert(yMin && yMax);

  constexpr int sampleSize = 80;

  float sampleYMin = FLT_MAX, sampleYMax = -FLT_MAX;
  const float step = (xMax - xMin) / (sampleSize - 1);
  float x, y;
  float sum = 0.f;
  int pop = 0;

  for (int i = 1; i < sampleSize; i++) {
    x = xMin + i * step;
    y = privateEvaluateXYAtParameter(x, context).x2();
    sampleYMin = std::min(sampleYMin, y);
    sampleYMax = std::max(sampleYMax, y);
    if (std::isfinite(y) && std::fabs(y) > FLT_EPSILON) {
      sum += std::log(std::fabs(y));
      pop++;
    }
  }
  /* sum/n is the log mean value of the function, which can be interpreted as
   * its average order of magnitude. Then, bound is the value for the next
   * order of magnitude and is used to cut the Y range. */
  float bound = (pop > 0) ? std::exp(sum / pop + 1.f) : FLT_MAX;
  *yMin = std::min(*yMin, std::max(sampleYMin, -bound));
  *yMax = std::max(*yMax, std::min(sampleYMax, bound));
  if (*yMin == *yMax) {
    float d = (*yMin == 0.f) ? 1.f : *yMin * 0.2f;
    *yMin -= d;
    *yMax += d;
  }
}

void * ContinuousFunction::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(RecordDataBuffer);
}

size_t ContinuousFunction::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size-sizeof(RecordDataBuffer);
}

ContinuousFunction::RecordDataBuffer * ContinuousFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
}

template<typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(T t, Poincare::Context * context) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(plotType() == PlotType::Cartesian ? t : NAN, NAN);
  }
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknown[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknown, bufferSize, UCodePointUnknown);
  PlotType type = plotType();
  Expression e = expressionReduced(context);
  if (type != PlotType::Parametric) {
    assert(type == PlotType::Cartesian || type == PlotType::Polar);
    return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(e, unknown, t, context));
  }
  assert(e.type() == ExpressionNode::Type::Matrix);
  assert(static_cast<Poincare::Matrix&>(e).numberOfRows() == 2);
  assert(static_cast<Poincare::Matrix&>(e).numberOfColumns() == 1);
  return Coordinate2D<T>(
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), unknown, t, context),
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), unknown, t, context));
}

Coordinate2D<double> ContinuousFunction::nextMinimumFrom(double start, double step, double max, Context * context) const {
  return nextPointOfInterestFrom(start, step, max, context, [](Expression e, char * symbol, double start, double step, double max, Context * context) { return PoincareHelpers::NextMinimum(e, symbol, start, step, max, context); });
}

Coordinate2D<double> ContinuousFunction::nextMaximumFrom(double start, double step, double max, Context * context) const {
  return nextPointOfInterestFrom(start, step, max, context, [](Expression e, char * symbol, double start, double step, double max, Context * context) { return PoincareHelpers::NextMaximum(e, symbol, start, step, max, context); });
}

Coordinate2D<double> ContinuousFunction::nextRootFrom(double start, double step, double max, Context * context) const {
  return nextPointOfInterestFrom(start, step, max, context, [](Expression e, char * symbol, double start, double step, double max, Context * context) { return Coordinate2D<double>(PoincareHelpers::NextRoot(e, symbol, start, step, max, context), 0.0); });
}

Coordinate2D<double> ContinuousFunction::nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, Poincare::Expression e, double eDomainMin, double eDomainMax) const {
  assert(plotType() == PlotType::Cartesian);
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknown);
  double domainMin = std::max<double>(tMin(), eDomainMin);
  double domainMax = std::min<double>(tMax(), eDomainMax);
  if (step > 0.0f) {
    start = std::max(start, domainMin);
    max = std::min(max, domainMax);
  } else {
    start = std::min(start, domainMax);
    max = std::max(max, domainMin);
  }
  return PoincareHelpers::NextIntersection(expressionReduced(context), unknownX, start, step, max, context, e);
}

Coordinate2D<double> ContinuousFunction::nextPointOfInterestFrom(double start, double step, double max, Context * context, ComputePointOfInterest compute) const {
  assert(plotType() == PlotType::Cartesian);
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknown);
  if (step > 0.0f) {
    start = std::max<double>(start, tMin());
    max = std::min<double>(max, tMax());
  } else {
    start = std::min<double>(start, tMax());
    max = std::max<double>(max, tMin());
  }
  return compute(expressionReduced(context), unknownX, start, step, max, context);
}

Poincare::Expression ContinuousFunction::sumBetweenBounds(double start, double end, Poincare::Context * context) const {
  assert(plotType() == PlotType::Cartesian);
  start = std::max<double>(start, tMin());
  end = std::min<double>(end, tMax());
  return Poincare::Integral::Builder(expressionReduced(context).clone(), Poincare::Symbol::Builder(UCodePointUnknown), Poincare::Float<double>::Builder(start), Poincare::Float<double>::Builder(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(const char * c, Poincare::Context * context) {
  setCache(nullptr);
  return ExpressionModelHandle::setContent(c, context);
}

template Coordinate2D<float> ContinuousFunction::templatedApproximateAtParameter<float>(float, Poincare::Context *) const;
template Coordinate2D<double> ContinuousFunction::templatedApproximateAtParameter<double>(double, Poincare::Context *) const;

template Poincare::Coordinate2D<float> ContinuousFunction::privateEvaluateXYAtParameter<float>(float, Poincare::Context *) const;
template Poincare::Coordinate2D<double> ContinuousFunction::privateEvaluateXYAtParameter<double>(double, Poincare::Context *) const;

}
