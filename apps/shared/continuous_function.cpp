#include "continuous_function.h"

#include <apps/apps_container_helper.h>
#include <escher/palette.h>
#include <poincare/code_points.h>
#include <poincare/helpers/symbol.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/new_trigonometry.h>
#include <poincare/numeric/roots.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/print.h>
#include <poincare/src/expression/derivation.h>

#include <algorithm>

#include "global_context.h"
#include "poincare_helpers.h"

using namespace Poincare;

namespace Shared {

// Symbols
constexpr CodePoint k_cartesianSymbol = Poincare::CodePoints::k_cartesianSymbol;
constexpr CodePoint k_parametricSymbol =
    Poincare::CodePoints::k_parametricSymbol;
constexpr CodePoint k_polarSymbol = Poincare::CodePoints::k_polarSymbol;
constexpr CodePoint k_radiusSymbol = Poincare::CodePoints::k_radiusSymbol;
constexpr CodePoint k_ordinateSymbol = Poincare::CodePoints::k_ordinateSymbol;
constexpr CodePoint k_unnamedExpressionSymbol = k_cartesianSymbol;

/* ContinuousFunction - Public */

ContinuousFunction ContinuousFunction::NewModel(
    Ion::Storage::Record::ErrorStatus* error, const char* baseName,
    KDColor color) {
  assert(baseName != nullptr);
  // Create the record
  /* WARNING: We create an empty record with the baseName and extension right
   * away because the baseName pointer might be corrupted later on.
   * Indeed, the baseName can point in the Pool, which can be altered during
   * the record creation, because the FunctionStore will have its memoization
   * reset.
   * Creating an empty record with the base name computes and saves the CRC32
   * of the name, so that the baseName variable is not needed anymore after
   * calling the method "createRecordWithExtension". */
  Ion::Storage::Record record =
      Ion::Storage::Record(baseName, Ion::Storage::functionExtension);
  RecordDataBuffer data(color);
  *error =
      Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
          baseName, Ion::Storage::functionExtension, &data, sizeof(data));
  // Return the ContinuousFunction with the new record
  return ContinuousFunction(
      *error == Ion::Storage::Record::ErrorStatus::None ? record : Record());
}

ContinuousFunction::ContinuousFunction(Ion::Storage::Record record)
    : Function(record), m_cache(nullptr) {
  /* The name of the record might need an update after another expression
   * on which this one depended was updated.
   * Since each time a record is updated, all ContinuousFunctions are tidied
   * and will be re-built later, it's safe to do it in this constructor.
   * Example:
   *  Expression 1: f(x)=x^2
   *  Expression 2: f(x)=x
   *  Here f(x)=x is treated as the equation x^2=x and its record name is ?1.
   *  If Expression 1 is modified into: g(x)=x^2, the Expression 2 should
   *  now be updated as the function f(x), so the record needs a renaming.
   * */
  if (!record.isNull()) {
    Ion::Storage::Record::ErrorStatus error =
        updateNameIfNeeded(Poincare::Context::GlobalContext);
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    (void)error;
  }
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::updateNameIfNeeded(
    Context* context) {
  return m_model.renameRecordIfNeeded(this, context);
}

size_t ContinuousFunction::nameWithoutArgument(char* buffer, size_t bufferSize,
                                               int derivationOrder) {
  assert(0 <= derivationOrder && derivationOrder <= 2);
  size_t length = 0;
  if (isNamed()) {
    length += Function::name(buffer, bufferSize);
  } else {
    length += SerializationHelper::CodePoint(
        buffer, bufferSize,
        properties().isPolar()
            ? k_radiusSymbol
            : (properties().isInversePolar() ? k_polarSymbol
                                             : k_ordinateSymbol));
  }
  if (derivationOrder > 0) {
    const CodePoint derivative =
        derivationOrder == 1
            ? Poincare::Internal::Derivation::k_firstOrderSymbol
            : Poincare::Internal::Derivation::k_secondOrderSymbol;
    length += SerializationHelper::CodePoint(buffer + length,
                                             bufferSize - length, derivative);
  }
  return length;
}

size_t ContinuousFunction::nameWithArgument(char* buffer, size_t bufferSize,
                                            int derivationOrder) {
  assert(0 <= derivationOrder && derivationOrder <= 2);
  size_t length = nameWithoutArgument(buffer, bufferSize, derivationOrder);
  if (isNamed()) {
    length += withArgument(buffer + length, bufferSize - length);
  }
  return length;
}

size_t ContinuousFunction::printAbscissaValue(double cursorT, double cursorX,
                                              char* buffer, size_t bufferSize,
                                              int precision) {
  ContinuousFunctionProperties thisProperties = properties();
  /* With Vertical curves, cursorT != cursorX .
   * We need the value for symbol=... */
  return PoincareHelpers::ConvertFloatToText<double>(
      thisProperties.isCartesian() || thisProperties.isScatterPlot() ? cursorX
                                                                     : cursorT,
      buffer, bufferSize, precision);
}

size_t ContinuousFunction::printFunctionValue(double cursorT, double cursorX,
                                              double cursorY, char* buffer,
                                              size_t bufferSize, int precision,
                                              Context* context) {
  ContinuousFunctionProperties thisProperties = properties();
  if (thisProperties.isParametric()) {
    Preferences::PrintFloatMode mode =
        Poincare::Preferences::SharedPreferences()->displayMode();
    return Poincare::Print::CustomPrintf(buffer, bufferSize, "(%*.*ed;%*.*ed)",
                                         cursorX, mode, precision, cursorY,
                                         mode, precision);
  }
  double value = thisProperties.isPolar() || thisProperties.isInversePolar()
                     ? evaluate2DAtParameter(cursorT, context).y()
                     : cursorY;
  return PoincareHelpers::ConvertFloatToText<double>(value, buffer, bufferSize,
                                                     precision);
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(
    const Poincare::Layout& l, Context* context) {
  setCache(nullptr);
  bool wasCartesian = properties().isCartesian();
  /* About to set the content, the symbol does not matter here yet. We don't use
   * ExpressionModelHandle::setContent implementation to avoid calling symbol()
   * and any unnecessary plot type update at this point. See comment in
   * ContinuousFunction::Model::buildExpressionFromLayout. */
  Ion::Storage::Record::ErrorStatus error =
      m_model.setContent(this, l, context, k_unnamedExpressionSymbol);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    // Set proper name
    error = updateNameIfNeeded(context);
    // Update model
    updateModel(context, wasCartesian);
  }
  return error;
}

void ContinuousFunction::tidyDownstreamPoolFrom(
    PoolObject* treePoolCursor) const {
  ExpressionModelHandle::tidyDownstreamPoolFrom(treePoolCursor);
  m_cache = nullptr;
}

bool ContinuousFunction::isNamed() const {
  // Unnamed functions have a fullname starting with k_unnamedRecordFirstChar
  const char* recordFullName = fullName();
  return recordFullName != nullptr &&
         recordFullName[0] != k_unnamedRecordFirstChar;
}

bool ContinuousFunction::isDiscontinuousBetweenFloatValues(
    float x1, float x2, Poincare::Context* context) const {
  SystemFunction equation =
      expressionReduced(context).getSystemFunction(k_unknownName);
  return equation.isDiscontinuousBetweenFloatValues(x1, x2);
}

void ContinuousFunction::getLineParameters(double* slope, double* intercept,
                                           Context* context) const {
  assert(properties().isLine());
  SystemExpression equation = expressionReduced(context);
  // Compute metrics for details view of Line
  SystemExpression
      coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Separate the two line coefficients for approximation.
  int d = equation.getPolynomialReducedCoefficients(
      k_unknownName, coefficients, context, complexFormat(context),
      Poincare::Preferences::SharedPreferences()->angleUnit(),
      ContinuousFunctionProperties::k_defaultUnitFormat,
      SymbolicComputation::ReplaceAllSymbols);
  assert(d <= 1);
  /* Degree might vary depending on symbols definition and complex format.
   * Approximate and return the two line coefficients */
  if (d < 0) {
    *slope = NAN;
    *intercept = NAN;
  } else {
    *intercept = coefficients[0].approximateToScalar<double>();
    if (d == 0) {
      *slope = 0.0;
    } else {
      *slope = coefficients[1].approximateToScalar<double>();
    }
  }
}

CartesianConic ContinuousFunction::cartesianConicParameters(
    Context* context) const {
  assert(properties().isConic() && properties().isCartesian());
  return CartesianConic(expressionReducedForAnalysis(context), k_unknownName);
}

KDColor ContinuousFunction::subCurveColor(int subCurveIndex) const {
  return color(derivationOrderFromSubCurveIndex(subCurveIndex));
}

double ContinuousFunction::evaluateCurveParameter(int index, double cursorT,
                                                  double cursorX,
                                                  double cursorY,
                                                  Context* context) const {
  switch (properties().symbolType()) {
    case ContinuousFunctionProperties::SymbolType::T:
      return index == 0   ? cursorT
             : index == 1 ? evaluateXYAtParameter(cursorT, context).x()
                          : evaluateXYAtParameter(cursorT, context).y();
    case ContinuousFunctionProperties::SymbolType::Theta:
    case ContinuousFunctionProperties::SymbolType::Radius:
      return index == 0 ? cursorT : evaluate2DAtParameter(cursorT, context).y();
    default:
      return index == 0 ? cursorX : cursorY;
  }
}

void ContinuousFunction::updateModel(Context* context, bool wasCartesian) {
  setCache(nullptr);
  m_model.resetProperties();  // Reset model's properties.
  properties();               // update properties.
  if (wasCartesian != properties().isCartesian() ||
      !properties().canHaveCustomDomain()) {
    // The definition's domain must be reset.
    setTAuto(true);
  }
  if (!canDisplayDerivative()) {
    // No need to setDisplayValues, it is already included in setDisplayPlot
    setDisplayPlotFirstDerivative(false);
    setDisplayPlotSecondDerivative(false);
  }
}

void ContinuousFunction::updateDerivativeColorAfterChangingPlotDisplay(
    bool newDisplay, int derivationOrder) const {
  /* - if we are displaying the plot: use a new color
   * - if we are hidding the plot: use the color of the main function */
  KDColor newColor =
      newDisplay ? GlobalContext::s_continuousFunctionStore->colorForNewModel()
                 : color();
  recordData()->setColor(newColor, derivationOrder);
}

void ContinuousFunction::setDisplayValueFirstDerivative(bool display) {
  recordData()->setDisplayValueFirstDerivative(display);
}

void ContinuousFunction::setDisplayPlotFirstDerivative(bool display) {
  /* Call recordData()->displayPlotFirstDerivative() instead of
   * displayPlotFirstDerivative() to avoid the assert. */
  bool previousDisplay = recordData()->displayPlotFirstDerivative();
  recordData()->setDisplayPlotFirstDerivative(display);
  recordData()->setDisplayValueFirstDerivative(display);
  if (previousDisplay != display) {
    updateDerivativeColorAfterChangingPlotDisplay(display, 1);
  }
}

void ContinuousFunction::setDisplayValueSecondDerivative(bool display) {
  recordData()->setDisplayValueSecondDerivative(display);
}

void ContinuousFunction::setDisplayPlotSecondDerivative(bool display) {
  /* Call recordData()->displayPlotSecondDerivative() instead of
   * displayPlotSecondDerivative() to avoid the assert. */
  bool previousDisplay = recordData()->displayPlotSecondDerivative();
  recordData()->setDisplayPlotSecondDerivative(display);
  recordData()->setDisplayValueSecondDerivative(display);
  if (previousDisplay != display) {
    updateDerivativeColorAfterChangingPlotDisplay(display, 2);
  }
}

int ContinuousFunction::derivationOrderFromRelativeIndex(
    int relativeIndex, DerivativeDisplayType type) const {
  bool displayFirstDerivative, displaySecondDerivative;
  if (type == DerivativeDisplayType::Plot) {
    displayFirstDerivative = displayPlotFirstDerivative();
    displaySecondDerivative = displayPlotSecondDerivative();
  } else {
    assert(type == DerivativeDisplayType::Value);
    displayFirstDerivative = displayValueFirstDerivative();
    displaySecondDerivative = displayValueSecondDerivative();
  }
  switch (relativeIndex) {
    case 0:
      return 0;
    case 1:
      assert(displayFirstDerivative || displaySecondDerivative);
      return displayFirstDerivative ? 1 : 2;
    default:
      assert(relativeIndex == 2 && displayFirstDerivative &&
             displaySecondDerivative);
      return 2;
  }
  (void)displaySecondDerivative;
}

int ContinuousFunction::derivationOrderFromSubCurveIndex(
    int subCurveIndex) const {
  assert(numberOfSubCurves() == 0 ||
         (0 <= subCurveIndex && subCurveIndex < numberOfSubCurves(true)));
  return numberOfSubCurves() > 1
             ? 0
             : derivationOrderFromRelativeIndex(subCurveIndex,
                                                DerivativeDisplayType::Plot);
}

int ContinuousFunction::subCurveIndexFromDerivationOrder(
    int derivationOrder) const {
  int subCurveIndex;
  switch (derivationOrder) {
    case 0:
      subCurveIndex = 0;
      break;
    case 1:
      assert(displayPlotFirstDerivative());
      subCurveIndex = 1;
      break;
    default:
      assert(derivationOrder == 2);
      assert(displayPlotSecondDerivative());
      subCurveIndex = displayPlotFirstDerivative() ? 2 : 1;
      break;
  }
  assert(0 <= subCurveIndex && subCurveIndex < numberOfSubCurves(true));
  return subCurveIndex;
}

void ContinuousFunction::valuesToDisplayOnDerivativeCurve(
    int derivationOrder, bool* image, bool* firstDerivative,
    bool* secondDerivative) const {
  *image = false;
  *firstDerivative = false;
  *secondDerivative = false;
  switch (derivationOrder) {
    case 0:
      *image = true;
      *firstDerivative = displayValueFirstDerivative();
      *secondDerivative = displayValueSecondDerivative();
      return;
    case 1:
      assert(displayPlotFirstDerivative());
      *firstDerivative = true;
      return;
    case 2:
      assert(displayPlotSecondDerivative());
      *secondDerivative = true;
      return;
  }
}

template <typename T>
PointOrScalar<T> ContinuousFunction::approximateDerivative(
    T t, Context* context, int derivationOrder, bool useDomain) const {
  assert(canDisplayDerivative());
  assert(!isAlongY());
  assert(numberOfSubCurves() == 1);
  if (useDomain && (t < tMin() || t > tMax())) {
    if (properties().isParametric()) {
      return PointOrScalar<T>(NAN, NAN);
    }
    return PointOrScalar<T>(NAN);
  }
  // Derivative is simplified once and for all
  SystemFunction derivate = expressionDerivateReduced(context, derivationOrder)
                                .getSystemFunction(k_unknownName);
  return derivate.approximateToPointOrScalarWithValue(t);
}

double ContinuousFunction::approximateSlope(double t,
                                            Poincare::Context* context) const {
  if (t < tMin() || t > tMax()) {
    return NAN;
  }
  // Slope is simplified once and for all
  return expressionSlopeReduced(context).approximateToScalarWithValue(t);
}

void ContinuousFunction::setTMin(float tMin) {
  assert(!recordData()->tAuto());
  recordData()->setTMin(tMin);
  setCache(nullptr);
}

void ContinuousFunction::setTMax(float tMax) {
  assert(!recordData()->tAuto());
  recordData()->setTMax(tMax);
  setCache(nullptr);
}

void ContinuousFunction::setTAuto(bool tAuto) {
  if (!tAuto && !recordData()->tAuto()) {
    return;
  }
  /* Domain either was or will be auto. Reset values anyway in case model has
   * been updated or angle unit changed. */
  recordData()->setTAuto(tAuto);
  setCache(nullptr);
  if (tAuto) {
    // No need to update Tmin or Tmax since the auto value will be returned
    return;
  }
  recordData()->setTMin(autoTMin());
  recordData()->setTMax(autoTMax());
}

float ContinuousFunction::autoTMax() const {
  return properties().isCartesian() || properties().isScatterPlot()
             ? INFINITY
             : (properties().isInversePolar()
                    ? Range1D<float>::k_defaultHalfLength
                    : 2.f * NewTrigonometry::PiInAngleUnit(
                                Preferences::SharedPreferences()->angleUnit()));
}

float ContinuousFunction::autoTMin() const {
  return properties().isCartesian()
             ? -INFINITY
             : (properties().isInversePolar()
                    ? -Range1D<float>::k_defaultHalfLength
                    : 0.f);
}

bool ContinuousFunction::approximationBasedOnCostlyAlgorithms(
    Context* context) const {
  return expressionApproximated(context).recursivelyMatches(
      [](const NewExpression e) {
        return !e.isUninitialized() &&
               (e.isSequence() || e.isIntegral() || e.isDiff());
      });
}

void ContinuousFunction::trimResolutionInterval(double* start,
                                                double* end) const {
  double tmin = tMin(), tmax = tMax();
  *start = *start < tmin ? tmin : tmax < *start ? tmax : *start;
  *end = *end < tmin ? tmin : tmax < *end ? tmax : *end;
}

UserExpression ContinuousFunction::sumBetweenBounds(double start, double end,
                                                    Context* context) const {
  assert(properties().isCartesian());
  start = std::max<double>(start, tMin());
  end = std::min<double>(end, tMax());
  // Integral takes ownership of args
  return UserExpression::Create(KIntegral(KUnknownSymbol, KA, KB, KC),
                                {.KA = NewExpression::Builder<double>(start),
                                 .KB = NewExpression::Builder<double>(end),
                                 .KC = expressionReduced(context)});
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
}

/* ContinuousFunction - Private */

float ContinuousFunction::rangeStep() const {
  return properties().isCartesian()
             ? NAN
             : (tMax() - tMin()) / k_polarParamRangeSearchNumberOfPoints;
}

template <typename T>
Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(
    T t, Context* context, int subCurveIndex) const {
  ContinuousFunctionProperties thisProperties = properties();
  Coordinate2D<T> x1x2 =
      templatedApproximateAtParameter(t, context, subCurveIndex);
  if (thisProperties.isParametric() || thisProperties.isCartesian() ||
      thisProperties.isScatterPlot()) {
    return x1x2;
  }
  assert(thisProperties.isPolar() || thisProperties.isInversePolar());
  const T r = thisProperties.isPolar() ? x1x2.y() : x1x2.x();
  const T angle = (thisProperties.isPolar() ? x1x2.x() : x1x2.y()) * M_PI /
                  NewTrigonometry::PiInAngleUnit(
                      Poincare::Preferences::SharedPreferences()->angleUnit());
  return Coordinate2D<T>(r * std::cos(angle), r * std::sin(angle));
}

template <typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(
    T t, Context* context, int subCurveIndex) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(properties().isCartesian() ? t : NAN, NAN);
  }
  int derivationOrder = derivationOrderFromSubCurveIndex(subCurveIndex);
  SystemFunction e = expressionApproximated(context, derivationOrder);

  if (properties().isScatterPlot()) {
    assert(e.dimension().isPointOrListOfPoints() ||
           e.dimension().isEmptyList());
    SystemExpression point;
    if (e.dimension().isPoint()) {
      if (t != static_cast<T>(0.)) {
        return Coordinate2D<T>();
      }
      point = e;
    } else {
      assert(e.dimension().isListOfPoints() || e.dimension().isEmptyList());
      int tInt = t;
      if (static_cast<T>(tInt) != t || tInt < 0 ||
          tInt >= static_cast<List&>(e).numberOfChildren()) {
        return Coordinate2D<T>();
      }
      point = e.cloneChildAtIndex(tInt);
    }
    assert(!point.isUninitialized() && point.dimension().isPoint());
    if (point.isUndefined()) {
      return Coordinate2D<T>();
    }
    return point.approximateToPoint<T>();
  }

  if (!properties().isParametric()) {
    if (numberOfSubCurves() >= 2) {
      assert(subCurveIndex >= 0);
      assert(derivationOrder == 0);
      assert(e.isList());
      assert(static_cast<List&>(e).numberOfChildren() > subCurveIndex);
    }
    T value = e.approximateToScalarWithValue<T>(t, subCurveIndex);
    if (isAlongY()) {
      // Invert x and y with vertical lines so it can be scrolled vertically
      return Coordinate2D<T>(value, t);
    }
    return Coordinate2D<T>(t, value);
  }
  if (e.isUndefined()) {
    return Coordinate2D<T>();
  }
  assert(e.isPoint());
  return e.approximateToPointOrScalarWithValue<T>(t).toPoint();
}

ContinuousFunction::RecordDataBuffer::RecordDataBuffer(KDColor color)
    : Shared::Function::RecordDataBuffer(color),
      m_domain(-INFINITY, INFINITY),
      m_derivativesOptions{},
      m_tAuto(true) {
  m_derivativesOptions.colorFirstDerivative = color;
  m_derivativesOptions.colorSecondDerivative = color;
}

KDColor ContinuousFunction::RecordDataBuffer::color(int derivationOrder) const {
  switch (derivationOrder) {
    case 0:
      return Function::RecordDataBuffer::color();
    case 1:
      return KDColor::RGB16(m_derivativesOptions.colorFirstDerivative);
    default:
      assert(derivationOrder == 2);
      return KDColor::RGB16(m_derivativesOptions.colorSecondDerivative);
  }
}

void ContinuousFunction::RecordDataBuffer::setColor(KDColor color,
                                                    int derivationOrder) {
  switch (derivationOrder) {
    case 0:
      Function::RecordDataBuffer::setColor(color);
      break;
    case 1:
      m_derivativesOptions.colorFirstDerivative = color;
      break;
    default:
      assert(derivationOrder == 2);
      m_derivativesOptions.colorSecondDerivative = color;
      break;
  }
}

/* ContinuousFunction::Model */

SystemExpression ContinuousFunction::Model::expressionReduced(
    const Ion::Storage::Record* record, Context* context) const {
  // m_expression might already be memmoized.
  if (m_expression.isUninitialized()) {
    // Retrieve the expression equation's expression.
    m_expression = expressionReducedForAnalysis(record, context);
    ContinuousFunctionProperties thisProperties = properties(record);
    if (!thisProperties.isEnabled()) {
      m_expression = Undefined::Builder();
      return m_expression;
    }
    Preferences::ComplexFormat complexFormat =
        this->complexFormat(record, context);
    Preferences::AngleUnit angleUnit =
        Preferences::SharedPreferences()->angleUnit();
    if (thisProperties.isScatterPlot()) {
      /* Scatter plots do not depend on any variable, so they can be
       * approximated in advance.
       * In addition, they are sorted to be travelled from left to right (i.e.
       * in order of ascending x). */
      if (m_expression.dimension().isListOfPoints()) {
        m_expression = m_expression.removeUndefListElements()
                           .approximateListAndSort<double>();
        // m_expression may be an empty, dimension-less list after this.
      } else {
        assert(m_expression.dimension().isPoint());
        m_expression = PoincareHelpers::Approximate<double>(
            m_expression, context,
            {.complexFormat = complexFormat, .angleUnit = angleUnit});
      }
    } else if (!thisProperties.isPolar() && !thisProperties.isInversePolar() &&
               (record->fullName() == nullptr ||
                record->fullName()[0] == k_unnamedRecordFirstChar)) {
      /* Polar, inversePolar and cartesian equations are unnamed. Here
       * only cartesian equations are processed. */
      /* Function isn't named, m_expression currently is an expression in y or x
       * such as y = x. We extract the solution by solving in y or x. */
      int yDegree = m_expression.polynomialDegree(
          ContinuousFunctionProperties::k_ordinateName);
      bool willBeAlongX = true;
      if (yDegree < 1 || yDegree > 2) {
        int xDegree = m_expression.polynomialDegree(k_unknownName);
        if (xDegree < 1 || xDegree > 2) {
          // Such degrees of equation in y and x are not handled.
          m_expression = Undefined::Builder();
          return m_expression;
        }
        // Equation can be plotted along y. For example : x=cos(y) or x^2=1
        willBeAlongX = false;
      }
      /* Solve the equation in y (or x if not willBeAlongX)
       * Symbols are replaced to simplify roots. */
      SystemExpression
          coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
      int degree = m_expression.getPolynomialReducedCoefficients(
          willBeAlongX ? ContinuousFunctionProperties::k_ordinateName
                       : k_unknownName,
          coefficients, context, complexFormat, angleUnit,
          ContinuousFunctionProperties::k_defaultUnitFormat,
          SymbolicComputation::ReplaceDefinedSymbols, true);

      if (degree == -1) {
        /* The reduction failed, so the expression is not reduced and
         * getPolynomialReducedCoefficients returned -1. */
        return m_expression;
      }
      assert(!willBeAlongX || degree == yDegree);
      ReductionContext reductionContext(context, complexFormat, angleUnit,
                                        Preferences::UnitFormat::Metric,
                                        ReductionTarget::SystemForAnalysis);
      if (degree == 1) {
        m_expression = SystemExpression::Builder(
            Poincare::Roots::Linear(coefficients[1], coefficients[0]));
      } else if (degree == 2) {
        // Equation is of degree 2, each root is a subcurve to plot.
        assert(m_properties.isOfDegreeTwo());
        Internal::Tree* list = Poincare::Roots::Quadratic(
            coefficients[2], coefficients[1], coefficients[0]);
        if (list->numberOfChildren() == 1) {
          // Flat conic
          list->removeNode();
        } else if (list->numberOfChildren() == 2) {
          /* Swap the equations since conics inequalities assume their equations
           * are in a certain order to make the distinction between inside and
           * outside. */
          list->child(0)->swapWithTree(list->child(1));
        }
        m_expression = SystemExpression::Builder(list);
      } else {
        /* TODO: We could handle simple equations of any degree by solving the
         * equation within the graph view bounds, to plot as many vertical or
         * horizontal lines as needed. */
        m_expression = Undefined::Builder();
        return m_expression;
      }
      if (!willBeAlongX && yDegree != 0) {
        // No need to replace anything if yDegree is 0
        m_expression.replaceSymbolWithExpression(
            Symbol::Builder(k_ordinateSymbol), Symbol::SystemSymbol());
      }
    } else {
      /* m_expression is resulting of a simplification with the target
       * SystemForAnalysis. But SystemForApproximation can be sometimes way
       * simpler than the one from SystemForAnalysis. For example (x+9)^6 is
       * fully developped in SystemForAnalysis, which results in approximation
       * inaccuracy. On the other hand, the expression (x+1)^2-x^2-2x-1 should
       * be developped so that we understand that it's equal to zero, and is
       * better handled by SystemForAnalysis. To solve this problem, we try to
       * simplify both ways and compare the number of nodes of each expression.
       * We take the one that has the less node. This is not ideal because an
       * expression with less node does not always mean a simpler expression,
       * but it's a good compromise for now.       */
      UserExpression equation = expressionEquation(record, context);
      if (!equation.isUninitialized()) {
        SystemExpression resultForApproximation =
            PoincareHelpers::CloneAndReduce(
                equation, context,
                {.complexFormat = complexFormat,
                 .updateComplexFormatWithExpression = false,
                 .target = ReductionTarget::SystemForApproximation,
                 .symbolicComputation = SymbolicComputation::KeepAllSymbols});
        assert(!resultForApproximation.isUninitialized());
        if (resultForApproximation.numberOfDescendants(true) <
            m_expression.numberOfDescendants(true)) {
          m_expression = resultForApproximation;
        }
      }
    }
  }
  return m_expression;
}

Poincare::SystemFunction ContinuousFunction::Model::expressionApproximated(
    const Ion::Storage::Record* record, Poincare::Context* context,
    int derivationOrder) const {
  assert(0 <= derivationOrder && derivationOrder <= 2);
  SystemFunction* approximated;
  switch (derivationOrder) {
    case 0:
      approximated = &m_expressionApproximated;
      break;
    case 1:
      approximated = &m_expressionFirstDerivateApproximated;
      break;
    default:
      assert(derivationOrder == 2);
      approximated = &m_expressionSecondDerivateApproximated;
      break;
  }
  if (approximated->isUninitialized()) {
    SystemExpression e =
        derivationOrder == 0
            ? expressionReduced(record, context)
            : expressionDerivateReduced(record, context, derivationOrder);
    // TODO: factorise the next line with other methods?
    *approximated =
        e.getSystemFunction(properties(record).isAlongY()
                                ? ContinuousFunctionProperties::k_ordinateName
                                : k_unknownName);
  }
  return *approximated;
}

Poincare::SystemExpression
ContinuousFunction::Model::expressionReducedForAnalysis(
    const Ion::Storage::Record* record, Poincare::Context* context) const {
  ContinuousFunctionProperties::SymbolType computedFunctionSymbol =
      ContinuousFunctionProperties::k_defaultSymbolType;
  ComparisonJunior::Operator computedEquationType =
      ContinuousFunctionProperties::k_defaultEquationType;
  bool isCartesianEquation = false;
  UserExpression equation =
      expressionEquation(record, context, &computedEquationType,
                         &computedFunctionSymbol, &isCartesianEquation);
  SystemExpression result;
  Preferences::ComplexFormat complexFormat =
      this->complexFormat(record, context);
  if (!equation.isUndefined()) {
    result = PoincareHelpers::CloneAndReduce(
        equation, context,
        {.complexFormat = complexFormat,
         .updateComplexFormatWithExpression = false,
         .target = ReductionTarget::SystemForAnalysis,
         // Symbols have already been replaced.
         .symbolicComputation = SymbolicComputation::KeepAllSymbols});
    assert(!result.isUninitialized());
  } else {
    result = SystemExpression::Create(KUndef, {});
  }
  /* TODO_PCJ: equation and result used to be a same Expression at this step.
   * Ensure this pool usage regression of still having equation in the pool is
   * acceptable. */
  if (!m_properties.isInitialized()) {
    // Use the computed equation to update the plot type.
    m_properties.update(result, originalEquation(record, UCodePointUnknown),
                        context, complexFormat, computedEquationType,
                        computedFunctionSymbol, isCartesianEquation);
  }
  return result;
}

UserExpression ContinuousFunction::Model::expressionClone(
    const Ion::Storage::Record* record) const {
  assert(record->fullName() != nullptr &&
         record->fullName()[0] != k_unnamedRecordFirstChar);
  UserExpression e = ExpressionModel::expressionClone(record);
  if (e.isUninitialized()) {
    return e;
  }
  return e.cloneChildAtIndex(1);
}

UserExpression ContinuousFunction::Model::originalEquation(
    const Ion::Storage::Record* record, CodePoint symbol) const {
  UserExpression unknownSymbolEquation =
      ExpressionModel::expressionClone(record);
  if (unknownSymbolEquation.isUninitialized() || symbol == UCodePointUnknown) {
    return unknownSymbolEquation;
  }
  return unknownSymbolEquation.replaceSymbolWithExpression(
      Symbol::SystemSymbol(), Symbol::Builder(symbol));
}

bool ContinuousFunction::IsFunctionAssignment(const UserExpression e) {
  if (!e.isEquality()) {
    return false;
  }
  const UserExpression leftExpression = e.cloneChildAtIndex(0);
  if (!leftExpression.isUserFunction()) {
    return false;
  }
  const UserExpression functionSymbol = leftExpression.cloneChildAtIndex(0);
  return SymbolHelper::IsSymbol(functionSymbol, k_cartesianSymbol) ||
         SymbolHelper::IsSymbol(functionSymbol, k_parametricSymbol) ||
         SymbolHelper::IsSymbol(functionSymbol, k_polarSymbol);
}

UserExpression ContinuousFunction::Model::expressionEquation(
    const Ion::Storage::Record* record, Context* context,
    ComparisonJunior::Operator* computedEquationType,
    ContinuousFunctionProperties::SymbolType* computedFunctionSymbol,
    bool* isCartesianEquation) const {
  UserExpression result = ExpressionModel::expressionClone(record);
  if (result.isUninitialized()) {
    return Undefined::Builder();
  }
  ContinuousFunctionProperties::SymbolType tempFunctionSymbol =
      ContinuousFunctionProperties::k_defaultSymbolType;
  if (!result.isComparison()) {
    if (result.dimension(context).isPointOrListOfPoints()) {
      if (computedFunctionSymbol) {
        *computedFunctionSymbol =
            ContinuousFunctionProperties::SymbolType::NoSymbol;
      }
      result.replaceSymbols(context);
      if (!result.isUninitialized()) {
        // Result is not circularly defined.
        return result;
      }
    }
    /* Happens when:
     * - The input text is too long and "f(x)=" can't be inserted.
     * - When inputting amiguous equations like "x+y>2>y".
     * - When result is uninitialized because of circular definition.  */
    return Undefined::Builder();
  }

  ComparisonJunior::Operator equationType = result.comparisonOperator();
  if (equationType == ComparisonJunior::Operator::NotEqual) {
    return Undefined::Builder();
  }
  if (computedEquationType) {
    *computedEquationType = equationType;
  }
  bool isUnnamedFunction = true;
  /* TODO_PCJ: leftExpression is later changed and should not be a clone of
   * result's child. */
  UserExpression leftExpression = result.cloneChildAtIndex(0);

  if (IsFunctionAssignment(result)) {
    // Ensure that function name is either record's name, or free
    assert(record->fullName() != nullptr);
    assert(leftExpression.isUserFunction());
    const char* functionName = SymbolHelper::GetName(leftExpression);
    const size_t functionNameLength = strlen(functionName);
    const UserExpression functionSymbol = leftExpression.cloneChildAtIndex(0);
    CodePoint codePointSymbol = CodePointForSymbol(functionSymbol);
    if (Shared::GlobalContext::SymbolAbstractNameIsFree(functionName) ||
        strncmp(record->fullName(), functionName, functionNameLength) == 0) {
      // Set the model's plot type.
      tempFunctionSymbol =
          ContinuousFunctionProperties::SymbolTypeForCodePoint(codePointSymbol);
      result = result.cloneChildAtIndex(1);
      isUnnamedFunction = false;
    } else {
      /* Function in left part of the equation refer to an already defined one.
       * Replace the symbol. */
      leftExpression = ExpressionModel::ReplaceSymbolWithUnknown(
          leftExpression, codePointSymbol);
    }
  } else if (SymbolHelper::IsSymbol(leftExpression, k_radiusSymbol) ||
             SymbolHelper::IsSymbol(leftExpression, k_polarSymbol)) {
    result = result.cloneChildAtIndex(1);
    tempFunctionSymbol = SymbolHelper::IsSymbol(leftExpression, k_polarSymbol)
                             ? ContinuousFunctionProperties::SymbolType::Radius
                             : ContinuousFunctionProperties::SymbolType::Theta;
    isUnnamedFunction = false;
  }
  if (computedFunctionSymbol) {
    *computedFunctionSymbol = tempFunctionSymbol;
  }
  if (isCartesianEquation) {
    *isCartesianEquation = isUnnamedFunction;
  }
  if (isUnnamedFunction) {
    result = Expression::Create(
        KSub(KA, KB),
        {.KA = leftExpression, .KB = result.cloneChildAtIndex(1)});
    /* Replace all y symbols with UCodePointTemporaryUnknown so that they are
     * not replaced if they had a predefined value. This will not replace the y
     * symbols nested in function, which is not a supported behavior anyway.
     * TODO: Make a consistent behavior calculation/additional_results using a
     *       VariableContext to temporary disable y's predefinition. */
    result = result.replaceSymbolWithExpression(
        Symbol::Builder(k_ordinateSymbol),
        Symbol::Builder(UCodePointTemporaryUnknown));
  }
  // Replace all defined symbols and functions to extract symbols
  result.replaceSymbols(context);

  if (result.isUninitialized()) {
    // result was Circularly defined
    return Undefined::Builder();
  }
  if (isUnnamedFunction) {
    result = result.replaceSymbolWithExpression(
        Symbol::Builder(UCodePointTemporaryUnknown),
        Symbol::Builder(k_ordinateSymbol));
  }
  assert(!result.isUninitialized());
  return result;
}

SystemExpression ContinuousFunction::Model::expressionDerivateReduced(
    const Ion::Storage::Record* record, Context* context,
    int derivationOrder) const {
  // Derivative isn't available on curves with multiple subcurves
  assert(numberOfSubCurves(record) == 1);
  assert(1 <= derivationOrder && derivationOrder <= 2);
  SystemExpression* derivative = derivationOrder == 1
                                     ? &m_expressionFirstDerivate
                                     : &m_expressionSecondDerivate;
  if (derivative->isUninitialized()) {
    *derivative = expressionReduced(record, context)
                      .getReducedDerivative(k_unknownName, derivationOrder);
  }
  return *derivative;
}

SystemFunctionScalar ContinuousFunction::Model::expressionSlopeReduced(
    const Ion::Storage::Record* record, Context* context) const {
  /* Slope is only needed for parametric and polar functions.
   * For cartesian function, it is the same as the derivative.
   * For curves with multiple subcurves and for inverse polar,
   * it is not available. */
  // TODO: assert(canComputeTangent());
  ContinuousFunctionProperties prop = properties(record);
  if (m_expressionSlope.isUninitialized()) {
    if (prop.isCartesian()) {
      m_expressionSlope = expressionDerivateReduced(record, context, 1)
                              .getSystemFunction(k_unknownName);
    } else {
      assert(prop.isParametric() || prop.isPolar());
      SystemExpression expression = parametricForm(record, context);
      assert(expression.isPoint());
      m_expressionSlope =
          SystemExpression::CreateReduce(
              KMult(KA, KPow(KB, -1_e)),
              {
                  .KA = expression.cloneChildAtIndex(1).getReducedDerivative(
                      k_unknownName),
                  .KB = expression.cloneChildAtIndex(0).getReducedDerivative(
                      k_unknownName),
              })
              .getSystemFunction(k_unknownName);
    }
  }
  return m_expressionSlope;
}

Ion::Storage::Record::ErrorStatus
ContinuousFunction::Model::renameRecordIfNeeded(Ion::Storage::Record* record,
                                                Context* context) const {
  /* Use ExpressionModel::expressionClone because it does not alter
   * the left-hand side of "f(x)=" and "f(t)=", which allows the name
   * of the function to be found. */
  UserExpression newExpression = ExpressionModel::expressionClone(record);
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  if (newExpression.isUninitialized()) {
    /* The expression is not set, the record will be named later or is already
     * correctly named by GlobalContext. */
    return error;
  }
  if (record->hasExtension(Ion::Storage::functionExtension)) {
    if (IsFunctionAssignment(newExpression)) {
      const UserExpression function = newExpression.cloneChildAtIndex(0);
      error = Ion::Storage::Record::SetBaseNameWithExtension(
          record, static_cast<const SymbolAbstract&>(function).name(),
          Ion::Storage::functionExtension);
      if (error != Ion::Storage::Record::ErrorStatus::NameTaken) {
        return error;
      }
      // Function's name is already taken, reset records name if needed.
      error = Ion::Storage::Record::ErrorStatus::None;
    }
    if (record->fullName()[0] == k_unnamedRecordFirstChar) {
      // Record is already unnamed (and hidden).
      return error;
    }
    // Rename record with a hidden record name.
    char name[k_maxDefaultNameSize];
    size_t length = SerializationHelper::CodePoint(name, k_maxDefaultNameSize,
                                                   k_unnamedRecordFirstChar);
    Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(
        name, length, k_maxDefaultNameSize, Ion::Storage::functionExtension);
    error = Ion::Storage::Record::SetBaseNameWithExtension(
        record, name, Ion::Storage::functionExtension);
  }
  return error;
}

CodePoint ContinuousFunction::Model::CodePointForSymbol(
    const UserExpression& symbol) {
  // Extract the CodePoint function's symbol. We know it is either x, t or θ
  assert(symbol.isUserSymbol());
  if (SymbolHelper::IsSymbol(symbol, k_cartesianSymbol)) {
    return k_cartesianSymbol;
  }
  if (SymbolHelper::IsSymbol(symbol, k_polarSymbol)) {
    return k_polarSymbol;
  }
  assert(SymbolHelper::IsSymbol(symbol, k_parametricSymbol));
  return k_parametricSymbol;
}

Poincare::UserExpression ContinuousFunction::Model::buildExpressionFromLayout(
    Poincare::Layout l, CodePoint symbol, Poincare::Context* context) const {
  /* The symbol parameter is discarded in this implementation. Either there is a
   * valid named left expression and the symbol will be extracted, either the
   * symbol should be the default symbol used in unnamed expressions. */
  assert(symbol == k_unnamedExpressionSymbol);
  // if l is uninitialized, we want to reinit the Expression
  if (l.isUninitialized() || l.isEmpty()) {
    return UserExpression();
  }
  /* Parse the expression to store as possible function assignment. */
  UserExpression expressionToStore =
      UserExpression::Parse(l, context, true, true);
  if (expressionToStore.isUninitialized()) {
    return expressionToStore;
  }
  // Check if the equation is of the form f(x)=...
  if (IsFunctionAssignment(expressionToStore)) {
    const UserExpression functionSymbol =
        expressionToStore.cloneChildAtIndex(0).cloneChildAtIndex(0);
    // Extract the CodePoint function's symbol. We know it is either x, t or θ
    // Override the symbol so that it can be replaced in the right expression
    symbol = CodePointForSymbol(functionSymbol);
    // Do not replace symbol in f(x)=
    expressionToStore = ExpressionModel::ReplaceSymbolWithUnknown(
        expressionToStore, symbol, true);
  } else {
    if (expressionToStore.recursivelyMatches(SymbolHelper::IsTheta)) {
      symbol = SymbolHelper::IsSymbol(expressionToStore.cloneChildAtIndex(0),
                                      k_polarSymbol)
                   ? k_radiusSymbol
                   : k_polarSymbol;
    }
    // Fallback on normal parsing (replace symbol with unknown)
    expressionToStore =
        ExpressionModel::buildExpressionFromLayout(l, symbol, context);
  }

  return expressionToStore;
}

void ContinuousFunction::Model::tidyDownstreamPoolFrom(
    PoolObject* treePoolCursor) const {
  if (treePoolCursor == nullptr ||
      m_expressionFirstDerivate.isDownstreamOf(treePoolCursor)) {
    m_expressionFirstDerivate = SystemExpression();
  }
  if (treePoolCursor == nullptr ||
      m_expressionFirstDerivateApproximated.isDownstreamOf(treePoolCursor)) {
    m_expressionFirstDerivateApproximated = SystemFunction();
  }
  if (treePoolCursor == nullptr ||
      m_expressionSecondDerivate.isDownstreamOf(treePoolCursor)) {
    m_expressionSecondDerivate = SystemExpression();
  }
  if (treePoolCursor == nullptr ||
      m_expressionSecondDerivateApproximated.isDownstreamOf(treePoolCursor)) {
    m_expressionSecondDerivateApproximated = SystemFunction();
  }
  if (treePoolCursor == nullptr ||
      m_expressionSlope.isDownstreamOf(treePoolCursor)) {
    m_expressionSlope = SystemFunctionScalar();
  }
  if (treePoolCursor == nullptr ||
      m_expressionApproximated.isDownstreamOf(treePoolCursor)) {
    m_expressionApproximated = SystemFunction();
  }
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

ContinuousFunctionProperties ContinuousFunction::Model::properties(
    const Ion::Storage::Record* record) const {
  if (!m_properties.isInitialized()) {
    // Computing the expression equation will update the function properties
    expressionReducedForAnalysis(record, Poincare::Context::GlobalContext);
  }
  assert(m_properties.isInitialized());
  return m_properties;
}

int ContinuousFunction::Model::numberOfSubCurves(
    const Ion::Storage::Record* record) const {
  ContinuousFunctionProperties prop = properties(record);
  if (prop.isCartesian()) {
    SystemExpression e =
        expressionReduced(record, Poincare::Context::GlobalContext);
    if (e.isList()) {
      assert(prop.isOfDegreeTwo());
      return static_cast<List&>(e).numberOfChildren();
    }
  }
  return 1;
}

SystemExpression ContinuousFunction::Model::parametricForm(
    const Ion::Storage::Record* record, Poincare::Context* context) const {
  ContinuousFunctionProperties prop = properties(record);
  assert(prop.isPolar() || prop.isInversePolar() || prop.isParametric());
  SystemExpression e = expressionReduced(record, context);
  if (prop.isPolar() || prop.isInversePolar()) {
    /* Turn r(θ) into f(θ) = (x(θ), y(θ)) with
     * - x(θ) = r(θ) * cos(θ)
     * - y(θ) = r(θ) * sin(θ)
     * And θ(r) into f(r) = (x(r), y(r)) with
     * - x(r) = r * cos(θ(r))
     * - y(r) = r * sin(θ(r)) */
    e = SystemExpression::CreateReduce(
        KPoint(KMult(KA, KTrig(KB, 0_e)), KMult(KA, KTrig(KB, 1_e))),
        {.KA = prop.isPolar() ? e : Symbol::SystemSymbol(),
         .KB = prop.isPolar() ? Symbol::SystemSymbol() : e});
  } else {
    assert(prop.isParametric());
    e = e.clone();
  }
  assert(e.isPoint());
  return e;
}

void* ContinuousFunction::Model::expressionAddress(
    const Ion::Storage::Record* record) const {
  return (char*)record->value().buffer + sizeof(RecordDataBuffer);
}

size_t ContinuousFunction::Model::expressionSize(
    const Ion::Storage::Record* record) const {
  return record->value().size - sizeof(RecordDataBuffer);
}

void ContinuousFunction::Model::setStorageChangeFlag() const {
  GlobalContext::s_continuousFunctionStore->setStorageChangeFlag(true);
}

template Coordinate2D<float>
ContinuousFunction::templatedApproximateAtParameter<float>(float, Context*,
                                                           int) const;
template Coordinate2D<double>
ContinuousFunction::templatedApproximateAtParameter<double>(double, Context*,
                                                            int) const;

template Coordinate2D<float>
ContinuousFunction::privateEvaluateXYAtParameter<float>(float, Context*,
                                                        int) const;
template Coordinate2D<double>
ContinuousFunction::privateEvaluateXYAtParameter<double>(double, Context*,
                                                         int) const;
template PointOrScalar<double> ContinuousFunction::approximateDerivative(
    double t, Context* context, int derivationOrder, bool useDomain) const;

}  // namespace Shared
