#include "continuous_function.h"

#include <apps/apps_container_helper.h>
#include <apps/global_preferences.h>
#include <escher/theme/components.h>
#include <omg/utf8_helper.h>
#include <poincare/analysis_expression.h>
#include <poincare/code_points.h>
#include <poincare/function_properties/integral.h>
#include <poincare/helpers/polynomial.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/numeric_solver/roots.h>
#include <poincare/print.h>
#include <poincare/src/expression/derivation.h>
#include <poincare/symbol_context.h>
#include <poincare/trigonometry.h>

#include <algorithm>

#include "global_store.h"
#include "poincare_helpers.h"

using namespace Poincare;
using namespace Poincare::CodePoints;

namespace Shared {

constexpr CodePoint k_unnamedExpressionSymbol = k_cartesianSymbol;

/* ContinuousFunction - Public */

ContinuousFunction ContinuousFunction::NewModel(
    Ion::Storage::Record::ErrorStatus* error, const char* baseName,
    Escher::ThemeComponent color) {
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
    Ion::Storage::Record::ErrorStatus error = updateNameIfNeeded(true);
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    (void)error;
  }
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::updateNameIfNeeded(
    bool isNewRecordCreation) {
  return m_model.renameRecordIfNeeded(this, isNewRecordCreation);
}

size_t ContinuousFunction::nameWithoutArgument(char* buffer, size_t bufferSize,
                                               int derivationOrder) const {
  assert(0 <= derivationOrder && derivationOrder <= 2);
  assert(!isDerivative());
  size_t length = 0;
  if (isNamed()) {
    length += Function::name(buffer, bufferSize);
  } else {
    length += UTF8Helper::WriteCodePoint(
        buffer, bufferSize,
        properties().isPolar()
            ? k_radiusSymbol
            : (properties().isInversePolar() ? k_polarSymbol
                                             : k_ordinateSymbol));
  }
  if (derivationOrder > 0) {
    // Add apostrophes for derivatives
    const CodePoint derivative =
        derivationOrder == 1
            ? Poincare::Internal::Derivation::k_firstOrderSymbol
            : Poincare::Internal::Derivation::k_secondOrderSymbol;
    length += UTF8Helper::WriteCodePoint(buffer + length, bufferSize - length,
                                         derivative);
  }
  return length;
}

size_t ContinuousFunction::nameWithArgument(char* buffer, size_t bufferSize,
                                            int derivationOrder) const {
  assert(0 <= derivationOrder && derivationOrder <= 2);
  if (isDerivative()) {
    // Parent will handle the name with the derivation order
    derivationOrder = this->derivationOrder();
    return parentFunction()->nameWithArgument(buffer, bufferSize,
                                              derivationOrder);
  }
  size_t length = nameWithoutArgument(buffer, bufferSize, derivationOrder);
  if (isNamed()) {
    length += withArgument(buffer + length, bufferSize - length);
  }
  return length;
}

Layout ContinuousFunction::derivativeTitleLayout() const {
  assert(isDerivative());
  constexpr size_t bufferSize = k_maxNameWithArgumentSize;
  char buffer[bufferSize];
  size_t length = nameWithArgument(buffer, bufferSize);
  Layout name = Layout::String(buffer, length);
  Layout leftHandSide = Layout::Concatenate(name, Layout::CodePoint('='));
  UserExpression equation = originalEquation();
  assert(!equation.isUninitialized() && equation.numberOfChildren() == 2);
  Layout rightHandSide = PoincareHelpers::CreateLayout(
      equation.cloneChildAtIndex(1), GlobalContextAccessor::Context());
  return Layout::Concatenate(leftHandSide, rightHandSide);
}

size_t ContinuousFunction::printAbscissaValue(double cursorT, double cursorX,
                                              char* buffer, size_t bufferSize,
                                              int precision) const {
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
                                              size_t bufferSize,
                                              int precision) const {
  ContinuousFunctionProperties thisProperties = properties();
  if (thisProperties.isParametric()) {
    Preferences::PrintFloatMode mode = SharedGlobalPrefs().displayMode();
    return Poincare::Print::CustomPrintf(buffer, bufferSize, "(%*.*ed;%*.*ed)",
                                         cursorX, mode, precision, cursorY,
                                         mode, precision);
  }
  double value = thisProperties.isPolar() || thisProperties.isInversePolar()
                     ? evaluate2DAtParameter(cursorT).y()
                     : cursorY;
  return PoincareHelpers::ConvertFloatToText<double>(value, buffer, bufferSize,
                                                     precision);
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(
    const Poincare::Layout& l, const SymbolContext& symbolContext) {
  tidyCache();
  bool wasCartesian = properties().isCartesian();
  /* About to set the content, the symbol does not matter here yet. We don't use
   * ExpressionModelHandle::setContent implementation to avoid calling symbol()
   * and any unnecessary plot type update at this point. See comment in
   * ContinuousFunction::Model::buildExpressionFromLayout. */
  Ion::Storage::Record::ErrorStatus error =
      m_model.setContent(this, l, symbolContext, k_unnamedExpressionSymbol);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    // Set proper name
    error = updateNameIfNeeded(false);
    // Update model
    updateModel(wasCartesian);
  }
  return error;
}
bool ContinuousFunction::isActiveInTable() const {
  if (!isActive() || !properties().canBeActiveInTable()) {
    return false;
  }
  if (!isDerivative()) {
    return true;
  }
  int order = derivationOrder();
  assert(order == 1 || order == 2);
  OMG::ExpiringPointer<ContinuousFunction> parent = parentFunction();
  assert(!parent->isNull() && parent->isActive() &&
         parent->properties().canBeActiveInTable());
  return parent->displayValueDerivative(order);
}

bool ContinuousFunction::isActiveInPlot() const {
  if (!isActive()) {
    return false;
  }
  if (!isDerivative()) {
    return true;
  }
  int order = derivationOrder();
  assert(order == 1 || order == 2);
  OMG::ExpiringPointer<ContinuousFunction> parent = parentFunction();
  assert(!parent->isNull() && parent->isActive());
  return parent->displayPlotDerivative(order);
}

bool ContinuousFunction::isNamed() const {
  // Unnamed functions have a fullname starting with k_unnamedRecordFirstChar
  const char* recordFullName = fullName();
  return recordFullName != nullptr &&
         recordFullName[0] != k_unnamedRecordFirstChar;
}

OMG::ExpiringPointer<ContinuousFunction> ContinuousFunction::parentFunction()
    const {
  return GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(
      parentRecord());
}

bool ContinuousFunction::isDiscontinuousOnFloatInterval(float minBound,
                                                        float maxBound) const {
  return expressionApproximated().isDiscontinuousOnInterval<float>(minBound,
                                                                   maxBound);
}

void ContinuousFunction::getLineParameters(double* slope,
                                           double* intercept) const {
  assert(properties().isLineEquation());
  const AnalysisExpression& equation = expressionReducedForAnalysis();
  /* Compute metrics for details view of Line, separate the two line
   * coefficients for approximation. */
  AnalysisExpression coefficients =
      equation.getPolynomialReducedCoefficients(k_unknownName);
  /* Degree might vary depending on symbols definition and complex format.
   * Approximate and return the two line coefficients */
  if (coefficients.isUninitialized()) {
    *slope = NAN;
    *intercept = NAN;
  } else {
    int degree = coefficients.numberOfChildren() - 1;
    assert(degree >= 0);
    *intercept = coefficients.cloneChildAtIndex(0)
                     .approximateAnalysisToRealScalar<double>();
    if (degree == 0) {
      *slope = 0.0;
    } else {
      assert(degree == PolynomialHelpers::k_lineDegree);
      *slope = coefficients.cloneChildAtIndex(1)
                   .approximateAnalysisToRealScalar<double>();
    }
  }
}

CartesianConic ContinuousFunction::cartesianConicParameters() const {
  assert(properties().isConic() && properties().isCartesian());
  return CartesianConic(expressionReducedForAnalysis(), k_unknownName);
}

void ContinuousFunction::updateModel(bool wasCartesian) {
  tidyCache();
  m_model.resetProperties();  // Reset model's properties.
  properties();               // update properties.
  if (isDerivative()) {
    // Early exit, derivatives are always cartesian and do not have derivatives
    assert(wasCartesian && properties().isCartesian());
    assert(!canDisplayDerivative());
    return;
  }
  if (wasCartesian != properties().isCartesian() ||
      !properties().canHaveCustomDomain()) {
    // The definition's domain must be reset.
    setTAuto(true);
  }
  if (!canDisplayDerivative()) {
    // No need to setDisplayValues, it is already included in setDisplayPlot
    Ion::Storage::Record savedRecord = *this;
    setDisplayPlotDerivative(1, false);
    /* setDisplayPlotDerivative may delete derivative records, triggering a
     * storage change that resets all memoized models including `this`. Use the
     * saved record to re-fetch before the next call. */
    GlobalContextAccessor::ContinuousFunctionStore()
        .modelForRecord(savedRecord)
        ->setDisplayPlotDerivative(2, false);
  } else {
    // Derivatives are depend on their parent's, update them
    updateDerivativeModels();
  }
}

void ContinuousFunction::setActiveDerivatives() {
  assert(canDisplayDerivative());
  ContinuousFunctionStore& store =
      GlobalContextAccessor::ContinuousFunctionStore();
  for (int i = 0; i < store.numberOfModels(); i++) {
    OMG::ExpiringPointer<ContinuousFunction> f =
        store.modelForRecord(store.recordAtIndex(i));
    if (f->isDerivative() && f->parentRecord() == *this) {
      f->setActive(isActive());
    }
  }
}

void ContinuousFunction::updateDerivativeModels() {
  assert(canDisplayDerivative());
  ContinuousFunctionStore& store =
      GlobalContextAccessor::ContinuousFunctionStore();
  for (int i = 0; i < store.numberOfModels(); i++) {
    OMG::ExpiringPointer<ContinuousFunction> f =
        store.modelForRecord(store.recordAtIndex(i));
    if (f->isDerivative() && f->parentRecord() == *this) {
      bool wasCartesian = f->properties().isCartesian();
      // properties() might have triggered a cache reset, re-fetch model
      f = store.modelForRecord(store.recordAtIndex(i));
      // Remove memoized expressions dependent on the parent
      f->tidyDownstreamPoolFrom(nullptr);
      // Update cache and properties
      f->updateModel(wasCartesian);
    }
  }
}

Ion::Storage::Record::ErrorStatus
ContinuousFunction::createOrDeleteDerivativeRecord(int order, bool oldPlot,
                                                   bool oldValue, bool newPlot,
                                                   bool newValue) {
  assert(order == 1 || order == 2);
  if ((newPlot || newValue) && !oldPlot && !oldValue) {
    Ion::Storage::Record::ErrorStatus error =
        GlobalContextAccessor::ContinuousFunctionStore().addDerivativeModel(
            *this, order);
    return error;
  } else if (!newPlot && !newValue && (oldPlot || oldValue)) {
    GlobalStore::DeleteDerivativeOfRecord(*this, order);
  }
  return Ion::Storage::Record::ErrorStatus::None;
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setDisplayPlotDerivative(
    int order, bool display) {
  // Values flag is coupled with the plot flag, set it to the same value.
  bool hasPlot = order == 1 ? recordData()->displayPlotFirstDerivative()
                            : recordData()->displayPlotSecondDerivative();
  bool hasValue = order == 1 ? recordData()->displayValueFirstDerivative()
                             : recordData()->displayValueSecondDerivative();
  /* createOrDeleteDerivativeRecord might trigger a memoization reset which
   * invalidates `this`. Save the record to restore the model later. */
  Ion::Storage::Record savedRecord = *this;
  Ion::Storage::Record::ErrorStatus error = createOrDeleteDerivativeRecord(
      order, hasPlot, hasValue, display, display);
  if (error != Ion::Storage::Record::ErrorStatus::None) {
    return error;
  }
  OMG::ExpiringPointer<ContinuousFunction> function =
      GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(
          savedRecord);
  if (order == 1) {
    function->recordData()->setDisplayPlotFirstDerivative(display);
    function->recordData()->setDisplayValueFirstDerivative(display);
  } else {
    function->recordData()->setDisplayPlotSecondDerivative(display);
    function->recordData()->setDisplayValueSecondDerivative(display);
  }
  return Ion::Storage::Record::ErrorStatus::None;
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setDisplayValueDerivative(
    int order, bool display) {
  bool hasPlot = order == 1 ? recordData()->displayPlotFirstDerivative()
                            : recordData()->displayPlotSecondDerivative();
  bool hasValue = order == 1 ? recordData()->displayValueFirstDerivative()
                             : recordData()->displayValueSecondDerivative();
  /* createOrDeleteDerivativeRecord might trigger a memoization reset which
   * invalidates `this` (which points into the cache). Re-fetch via
   * modelForRecord afterward. */
  Ion::Storage::Record savedRecord = *this;
  Ion::Storage::Record::ErrorStatus error = createOrDeleteDerivativeRecord(
      order, hasPlot, hasValue, hasPlot, display);
  if (error != Ion::Storage::Record::ErrorStatus::None) {
    return error;
  }
  OMG::ExpiringPointer<ContinuousFunction> function =
      GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(
          savedRecord);
  if (order == 1) {
    function->recordData()->setDisplayValueFirstDerivative(display);
  } else {
    function->recordData()->setDisplayValueSecondDerivative(display);
  }
  return Ion::Storage::Record::ErrorStatus::None;
}

void ContinuousFunction::valuesToDisplayOnCurve(bool* image,
                                                bool* firstDerivative,
                                                bool* secondDerivative) const {
  *image = true;
  *firstDerivative = displayValueFirstDerivative();
  *secondDerivative = displayValueSecondDerivative();
}

template <typename T>
PointOrRealScalar<T> ContinuousFunction::approximateDerivative(
    T t, int derivationOrder, bool useDomain) const {
  assert(canDisplayDerivative());
  assert(!isAlongY());
  assert(numberOfSubCurves() == 1);
  if (useDomain && (t < tMin() || t > tMax())) {
    if (properties().isParametric()) {
      return PointOrRealScalar<T>(NAN, NAN);
    }
    return PointOrRealScalar<T>(NAN);
  }
  // Derivative is simplified once and for all
  return expressionApproximated(derivationOrder)
      .approximateToPointOrRealScalarWithValue(t);
}

double ContinuousFunction::approximateSlope(double t) const {
  if (t < tMin() || t > tMax()) {
    return NAN;
  }
  // Slope is simplified once and for all
  return expressionSlopeReduced().approximateToRealScalarWithValue(t);
}

void ContinuousFunction::setTMin(float tMin) {
  assert(!recordData()->tAuto());
  recordData()->setTMin(tMin);
  tidyCache();
}

void ContinuousFunction::setTMax(float tMax) {
  assert(!recordData()->tAuto());
  recordData()->setTMax(tMax);
  tidyCache();
}

void ContinuousFunction::setTAuto(bool tAuto) {
  if (!tAuto && !recordData()->tAuto()) {
    return;
  }
  /* Domain either was or will be auto. Reset values anyway in case model has
   * been updated or angle unit changed. */
  recordData()->setTAuto(tAuto);
  tidyCache();
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
                    : 2.f * Trigonometry::PiInAngleUnit(
                                SharedGlobalPrefs().angleUnit()));
}

float ContinuousFunction::autoTMin() const {
  return properties().isCartesian()
             ? -INFINITY
             : (properties().isInversePolar()
                    ? -Range1D<float>::k_defaultHalfLength
                    : 0.f);
}

bool ContinuousFunction::approximationBasedOnCostlyAlgorithms() const {
  return expressionApproximated().approximationBasedOnCostlyAlgorithms();
}

void ContinuousFunction::trimResolutionInterval(double* start,
                                                double* end) const {
  double tmin = tMin(), tmax = tMax();
  *start = *start < tmin ? tmin : tmax < *start ? tmax : *start;
  *end = *end < tmin ? tmin : tmax < *end ? tmax : *end;
}

SystemExpression ContinuousFunction::sumBetweenBounds(double start,
                                                      double end) const {
  assert(properties().isCartesian());
  // KUnknownSymbol is the variable used in the grapher app
  return Poincare::IntegralBetweenBounds(
      expressionReduced(), SymbolHelper::GetName(KUnknownSymbol),
      SystemExpression::Builder(std::max<double>(start, tMin())),
      SystemExpression::Builder(std::min<double>(end, tMax())));
}

/* ContinuousFunction - Private */

float ContinuousFunction::rangeStep() const {
  return properties().isCartesian()
             ? NAN
             : (tMax() - tMin()) / k_polarParamRangeSearchNumberOfPoints;
}

template <typename T>
Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(
    T t, int subCurveIndex) const {
  ContinuousFunctionProperties thisProperties = properties();
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, subCurveIndex);
  if (thisProperties.isParametric() || thisProperties.isCartesian() ||
      thisProperties.isScatterPlot()) {
    return x1x2;
  }
  assert(thisProperties.isPolar() || thisProperties.isInversePolar());
  const T r = thisProperties.isPolar() ? x1x2.y() : x1x2.x();
  const T angle = (thisProperties.isPolar() ? x1x2.x() : x1x2.y()) * M_PI /
                  Trigonometry::PiInAngleUnit(SharedGlobalPrefs().angleUnit());
  return Coordinate2D<T>(r * std::cos(angle), r * std::sin(angle));
}

template <typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(
    T t, int subCurveIndex) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(properties().isCartesian() ? t : NAN, NAN);
  }
  const PreparedFunction& e = expressionApproximated();
  if (properties().isScatterPlot()) {
    assert(e.dimension().isPointOrListOfPoints() ||
           e.dimension().isEmptyList());
    PreparedFunction point;
    if (e.dimension().isPoint()) {
      if (t != static_cast<T>(0.)) {
        return Coordinate2D<T>();
      }
      point = e;
    } else {
      assert(e.dimension().isListOfPoints() || e.dimension().isEmptyList());
      int tInt = t;
      if (static_cast<T>(tInt) != t || tInt < 0 ||
          tInt >= e.numberOfChildren()) {
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
      assert(!isDerivative());
      assert(e.isList());
      assert(e.numberOfChildren() > subCurveIndex);
    }
    T value = e.approximateToRealScalarWithValue<T>(t, subCurveIndex);
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
  return e.approximateToPointOrRealScalarWithValue<T>(t).toPoint();
}

ContinuousFunction::RecordDataBuffer::RecordDataBuffer(
    Escher::ThemeComponent color)
    : Shared::Function::RecordDataBuffer(color),
      m_domain(-INFINITY, INFINITY),
      m_derivativesOptions{},
      m_tAuto(true),
      m_derivationOrder(0),
      m_parentRecordCRC(0) {}

/* ContinuousFunction::Model */

const SystemExpression& ContinuousFunction::Model::expressionReduced(
    const Ion::Storage::Record* record) const {
  // m_expression might already be memmoized.
  if (!m_expression.isUninitialized()) {
    return m_expression;
  }
  if (expressionEquation(record).equation.isUninitialized()) {
    assert(m_expression.isUninitialized());
    return m_expression;
  }
  ContinuousFunctionProperties thisProperties = properties(record);
  if (!thisProperties.isEnabled()) {
    m_expression = SystemExpression::Undefined();
    return m_expression;
  }
  SimplificationParameters simplificationParameters = {
      .user = {.complexFormat = complexFormat(record),
               .angleUnit = SharedGlobalPrefs().angleUnit()},
      .projectionPolicy = {.symbolic = SymbolicComputation::KeepAllSymbols},
      .reductionPolicy = {.target = ReductionTarget::System}};
  if (thisProperties.isScatterPlot()) {
    // Clone and reduce equation
    m_expression = expressionEquation(record).equation.cloneAndReduce(
        simplificationParameters);
    /* Scatter plots do not depend on any variable, so they can be approximated
     * in advance. In addition, they are sorted to be traveled from left to
     * right (i.e. in order of ascending x). */
    if (m_expression.dimension().isListOfPoints()) {
      m_expression = m_expression.removeUndefAndComplexListElements()
                         .approximateListAndSort(FloatPrecision::Double);
      // m_expression may be an empty, dimension-less list after this.
    } else {
      assert(m_expression.dimension().isPoint());
      m_expression =
          m_expression.approximateSystemToTree(FloatPrecision::Double);
    }
  } else if (!thisProperties.isPolar() && !thisProperties.isInversePolar() &&
             (record->fullName() == nullptr ||
              record->fullName()[0] == k_unnamedRecordFirstChar)) {
    /* Polar, inversePolar and cartesian equations are unnamed. Here only
     * cartesian equations are processed. */
    /* Function isn't named, m_expression currently is an expression in y or x
     * such as y = x. We extract the solution by solving in y or x. */
    int yDegree = expressionReducedForAnalysis(record).polynomialDegree(
        ContinuousFunctionProperties::k_ordinateName);
    bool willBeAlongX = true;
    if (yDegree < 1 || yDegree > 2) {
      int xDegree =
          expressionReducedForAnalysis(record).polynomialDegree(k_unknownName);
      if (xDegree < 1 || xDegree > 2) {
        // Such degrees of equation in y and x are not handled.
        m_expression = SystemExpression::Undefined();
        return m_expression;
      }
      // Equation can be plotted along y. For example : x=cos(y) or x^2=1
      willBeAlongX = false;
    }
    if (yDegree == 1) {
      /* Escape simple y=f(x) equations, using System reduction
       * target to match f(x)= behavior.
       * TODO : implement a more general solution for any implicit curves. */
      UserExpression cartesianEquation =
          expressionEquation(record).equation.equivalentCartesianEquation();
      if (!cartesianEquation.isUninitialized()) {
        m_expression =
            cartesianEquation.cloneAndReduce(simplificationParameters);
        assert(!m_expression.isUninitialized() &&
               !m_expression.isUndefinedFailedSimplification());
        return m_expression;
      }
    }
    /* Solve the equation in y (or x if not willBeAlongX)
     * Symbols are replaced to simplify roots. */
    AnalysisExpression coefficients =
        expressionReducedForAnalysis(record).getPolynomialReducedCoefficients(
            willBeAlongX ? ContinuousFunctionProperties::k_ordinateName
                         : k_unknownName,
            true);
    assert(!coefficients.isUninitialized());
    int degree = coefficients.numberOfChildren() - 1;
    assert(degree >= 0 && degree <= 2);
    assert(!willBeAlongX || degree == yDegree);
    if (degree == 1) {
      m_expression = Poincare::Roots::Linear(coefficients.cloneChildAtIndex(1),
                                             coefficients.cloneChildAtIndex(0));
    } else if (degree == 2) {
      // Equation is of degree 2, each root is a subcurve to plot.
      assert(m_properties.isOfDegreeTwo());
      m_expression = Poincare::Roots::Quadratic(
          coefficients.cloneChildAtIndex(2), coefficients.cloneChildAtIndex(1),
          coefficients.cloneChildAtIndex(0));
    } else {
      /* TODO: We could handle simple equations of any degree by solving the
       * equation within the graph view bounds, to plot as many vertical or
       * horizontal lines as needed. */
      m_expression = SystemExpression::Undefined();
    }
  } else {
    m_expression = expressionEquation(record).equation.cloneAndReduce(
        simplificationParameters);
    assert(!m_expression.isUninitialized() &&
           !m_expression.isUndefinedFailedSimplification());
  }
  return m_expression;
}

const Poincare::PreparedFunction&
ContinuousFunction::Model::expressionApproximated(
    const Ion::Storage::Record* record, int derivationOrder) const {
  PreparedFunction* approximated;
  switch (derivationOrder) {
    case 1:
      approximated = &m_expressionFirstDerivateApproximated;
      break;
    case 2:
      approximated = &m_expressionSecondDerivateApproximated;
      break;
    default:
      assert(derivationOrder == 0);
      approximated = &m_expressionApproximated;
      break;
  }
  if (approximated->isUninitialized()) {
    const SystemExpression& e = expressionReduced(record);
    const char* symbolName = properties(record).isAlongY()
                                 ? ContinuousFunctionProperties::k_ordinateName
                                 : k_unknownName;
    if (derivationOrder > 0) {
      assert(!ContinuousFunction(*record).isDerivative());
      const SystemExpression derivative =
          e.getReducedDerivative(symbolName, derivationOrder);
      *approximated = derivative.getPreparedFunction(symbolName);
    } else {
      // TODO: factorise the next line with other methods?
      *approximated = e.getPreparedFunction(symbolName);
    }
  }
  return *approximated;
}

Poincare::AnalysisExpression
ContinuousFunction::Model::ComputeExpressionForAnalysis(
    const UserExpression& equation, ComplexFormat complexFormat,
    AngleUnit angleUnit) {
  AnalysisExpression result =
      equation
          .cloneAndReduce(SimplificationParameters{
              .user = {.complexFormat = complexFormat,
                       .angleUnit = angleUnit,
                       .context = GlobalContextAccessor::Context()},
              .projectionPolicy = {.symbolic =
                                       SymbolicComputation::KeepAllSymbols},
              .reductionPolicy = {.target = ReductionTarget::System}})
          .cloneAndExpand();
  assert(!result.isUninitialized());
  return result;
}

const Poincare::AnalysisExpression&
ContinuousFunction::Model::expressionReducedForAnalysis(
    const Ion::Storage::Record* record) const {
  if (!m_expressionForAnalysis.isUninitialized()) {
    return m_expressionForAnalysis;
  }
  EquationData equationData = expressionEquation(record);
  m_expressionForAnalysis = ComputeExpressionForAnalysis(
      equationData.equation, this->complexFormat(record),
      SharedGlobalPrefs().angleUnit());
  assert(!m_expressionForAnalysis.isUninitialized());
  return m_expressionForAnalysis;
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

const Internal::Tree* ContinuousFunction::Model::expressionTree(
    const Ion::Storage::Record* record) const {
  assert(record->fullName() != nullptr &&
         record->fullName()[0] != k_unnamedRecordFirstChar);
  assert(ExpressionModel::expressionTree(record));
  return ExpressionModel::expressionTree(record)->child(1);
}

UserExpression ContinuousFunction::Model::originalEquation(
    const Ion::Storage::Record* record, CodePoint symbol) const {
  OMG::ExpiringPointer<ContinuousFunction> f =
      GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(*record);
  if (!f->isDerivative()) {
    UserExpression equationWithUnknown = f->equationClone();
    if (equationWithUnknown.isUninitialized() || symbol == UCodePointUnknown) {
      return equationWithUnknown;
    }
    return equationWithUnknown.cloneAndReplaceUnknownWithSymbol(symbol);
  }
  // Derivative case
  int derivationOrder = f->derivationOrder();
  UserExpression parentEquation = f->parentFunction()->equationClone();
  assert(!parentEquation.isUninitialized());
  if (symbol != UCodePointUnknown) {
    parentEquation = parentEquation.cloneAndReplaceUnknownWithSymbol(symbol);
  }
  char symbolName[CodePoint::MaxCodePointCharLength + 1];
  size_t symbolNameLength =
      UTF8Helper::WriteCodePoint(symbolName, sizeof(symbolName) - 1, symbol);
  symbolName[symbolNameLength] = '\0';
  return parentEquation.getDerivative(symbolName, derivationOrder);
}

bool ContinuousFunction::IsFunctionAssignment(const UserExpression& e) {
  if (!e.isEquality()) {
    return false;
  }
  const UserExpression leftExpression = e.cloneChildAtIndex(0);
  if (!leftExpression.isUserFunction()) {
    return false;
  }
  const UserExpression functionSymbol = leftExpression.cloneChildAtIndex(0);
  return SymbolHelper::IsUserVariable(functionSymbol, k_cartesianSymbol) ||
         SymbolHelper::IsUserVariable(functionSymbol, k_parametricSymbol) ||
         SymbolHelper::IsUserVariable(functionSymbol, k_polarSymbol);
}

ContinuousFunction::Model::EquationData
ContinuousFunction::Model::expressionEquation(
    const Ion::Storage::Record* record) const {
  EquationData data = {
      .equation = UserExpression(),
      .equationType = ContinuousFunctionProperties::k_defaultEquationType,
      .functionSymbol = ContinuousFunctionProperties::k_defaultSymbolType,
      .isCartesianEquation = false,
  };
  UserExpression result = originalEquation(record, UCodePointUnknown);
  if (result.isUninitialized()) {
    data.equation = UserExpression::Undefined();
    return data;
  }
  if (!result.isComparison()) {
    if (result.dimension(GlobalContextAccessor::Context())
            .isPointOrListOfPoints()) {
      data.functionSymbol = ContinuousFunctionProperties::SymbolType::NoSymbol;
      result = result.cloneAndReplaceSymbols(GlobalContextAccessor::Context());
      if (!result.isUninitialized()) {
        // Result is not circularly defined.
        data.equation = result;
        return data;
      }
    }
    /* Happens when:
     * - The input text is too long and "f(x)=" can't be inserted.
     * - When inputting ambiguous equations like "x+y>2>y".
     * - When result is uninitialized because of circular definition.  */
    data.equation = UserExpression::Undefined();
    return data;
  }

  Comparison::Operator equationType = result.comparisonOperator();
  if (equationType == Comparison::Operator::NotEqual) {
    data.equation = UserExpression::Undefined();
    return data;
  }
  data.equationType = equationType;
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
    if (Shared::GlobalStore::UserNameIsFree(functionName) ||
        strncmp(record->fullName(), functionName, functionNameLength) == 0) {
      // Set the model's plot type.
      data.functionSymbol =
          ContinuousFunctionProperties::SymbolTypeForCodePoint(codePointSymbol);
      result = result.cloneChildAtIndex(1);
      isUnnamedFunction = false;
    } else {
      /* Function in left part of the equation refer to an already defined one.
       * Replace the symbol. */
      leftExpression = ExpressionModel::ReplaceSymbolWithUnknown(
          leftExpression, codePointSymbol);
    }
  } else if (SymbolHelper::IsUserVariable(leftExpression, k_radiusSymbol) ||
             SymbolHelper::IsUserVariable(leftExpression, k_polarSymbol)) {
    result = result.cloneChildAtIndex(1);
    data.functionSymbol =
        SymbolHelper::IsUserVariable(leftExpression, k_polarSymbol)
            ? ContinuousFunctionProperties::SymbolType::Radius
            : ContinuousFunctionProperties::SymbolType::Theta;
    isUnnamedFunction = false;
  }
  data.isCartesianEquation = isUnnamedFunction;
  if (isUnnamedFunction) {
    result = UserExpression::Create(
        KSub(KA, KB),
        {.KA = leftExpression, .KB = result.cloneChildAtIndex(1)});
    /* Replace all y symbols with UCodePointTemporaryUnknown so that they are
     * not replaced if they had a predefined value. This will not replace the y
     * symbols nested in function, which is not a supported behavior anyway.
     * TODO: Make a consistent behavior calculation/additional_results using a
     *       VariableContext to temporary disable y's predefinition. */
    result = result.cloneAndReplaceSymbolWithExpression(
        SymbolHelper::BuildUserVariable(k_ordinateSymbol),
        SymbolHelper::BuildUserVariable(UCodePointTemporaryUnknown));

    OMG::ExpiringPointer<ContinuousFunction> f =
        GlobalContextAccessor::ContinuousFunctionStore().modelForRecord(
            *record);
    if (f->isDerivative()) {
      /* A derivative's symbol is lost when it is changed to an equation, but it
       * can be found from its parent's symbol. */
      data.functionSymbol = f->parentFunction()->properties().symbolType();
    }
  }
  // Replace all defined symbols and functions to extract symbols
  result = result.cloneAndReplaceSymbols(GlobalContextAccessor::Context());

  if (result.isUninitialized()) {
    // result was Circularly defined
    data.equation = UserExpression::Undefined();
    return data;
  }
  if (isUnnamedFunction) {
    result = result.cloneAndReplaceSymbolWithExpression(
        SymbolHelper::BuildUserVariable(UCodePointTemporaryUnknown),
        SymbolHelper::BuildUserVariable(k_ordinateSymbol));
  }
  assert(!result.isUninitialized());
  data.equation = result;
  return data;
}

PreparedFunctionScalar ContinuousFunction::Model::expressionSlopeReduced(
    const Ion::Storage::Record* record) const {
  /* Slope is only needed for parametric and polar functions.
   * For cartesian function, it is the same as the derivative.
   * For curves with multiple subcurves and for inverse polar,
   * it is not available. */
  // TODO: assert(canComputeTangent());
  ContinuousFunctionProperties prop = properties(record);
  if (m_expressionSlope.isUninitialized()) {
    if (prop.isCartesian()) {
      m_expressionSlope = expressionApproximated(record, 1);
    } else {
      assert(prop.isParametric() || prop.isPolar());
      SystemExpression expression = parametricForm(record);
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
              .getPreparedFunction(k_unknownName);
    }
  }
  return m_expressionSlope;
}

Ion::Storage::Record::ErrorStatus
ContinuousFunction::Model::RenameRecordToHidden(Ion::Storage::Record* record) {
  char name[k_maxDefaultNameSize];
  size_t length = UTF8Helper::WriteCodePoint(name, k_maxDefaultNameSize,
                                             k_unnamedRecordFirstChar);
  Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(
      name, length, k_maxDefaultNameSize, Ion::Storage::functionExtension);
  Ion::Storage::Record oldRecord = *record;
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::SetBaseNameWithExtension(
          record, name, Ion::Storage::functionExtension);
  if (error == Ion::Storage::Record::ErrorStatus::None) {
    UpdateDerivativeRecordsWithNewParent(oldRecord, *record);
  }
  return error;
}

void ContinuousFunction::Model::UpdateDerivativeRecordsWithNewParent(
    Ion::Storage::Record oldParentRecord,
    Ion::Storage::Record newParentRecord) {
  ContinuousFunctionStore& store =
      GlobalContextAccessor::ContinuousFunctionStore();
  int n = store.numberOfModels();
  for (int i = 0; i < n; i++) {
    /* ContinuousFunction constructor calls updateNameIfNeeded, which may
     * trigger cache invalidation of the current record being renamed. We access
     * RecordDataBuffer directly to avoid that. */
    Ion::Storage::Record r = store.recordAtIndex(i);
    RecordDataBuffer* data = reinterpret_cast<RecordDataBuffer*>(
        const_cast<void*>(r.value().buffer));
    assert(data);
    if (data->derivationOrder() > 0 &&
        data->parentRecord() == oldParentRecord) {
      data->setParentRecord(newParentRecord);
    }
  }
}

Ion::Storage::Record::ErrorStatus
ContinuousFunction::Model::renameRecordIfNeeded(
    Ion::Storage::Record* record, bool isNewRecordCreation) const {
  /* Use equationClone because it preserves the left-hand side of "f(x)=" and
   * "f(t)=", which allows the name of the function to be found. */
  UserExpression newExpression = equationClone(record);
  if (newExpression.isUninitialized()) {
    if (isNewRecordCreation) {
      /* The expression is not set because we are in the process of creating a
       * new record. */
      return Ion::Storage::Record::ErrorStatus::None;
    }
    /* The expression is not set because an undefined expression has been set
     * to the record. We need to hide the record (notably in the varbox), this
     * is done by changing the record name to an "hidden" name. */
    return RenameRecordToHidden(record);
  }
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  if (record->hasExtension(Ion::Storage::functionExtension)) {
    if (IsFunctionAssignment(newExpression)) {
      const UserExpression function = newExpression.cloneChildAtIndex(0);
      const char* newName = SymbolHelper::GetName(function);
      Ion::Storage::Record oldRecord = *record;
      error = Ion::Storage::Record::SetBaseNameWithExtension(
          record, newName, Ion::Storage::functionExtension);
      if (error != Ion::Storage::Record::ErrorStatus::NameTaken) {
        if (error == Ion::Storage::Record::ErrorStatus::None &&
            oldRecord != *record) {
          /* Rename succeeded and record changed: update m_parentRecord on all
           * derivative records that reference this parent. */
          UpdateDerivativeRecordsWithNewParent(oldRecord, *record);
        }
        return error;
      }
      // Function's name is already taken, reset records name if needed.
      error = Ion::Storage::Record::ErrorStatus::None;
    }
    if (record->fullName()[0] == k_unnamedRecordFirstChar) {
      // Record is already unnamed (and hidden).
      return error;
    }
    error = RenameRecordToHidden(record);
  }
  return error;
}

CodePoint ContinuousFunction::Model::CodePointForSymbol(
    const UserExpression& symbol) {
  // Extract the CodePoint function's symbol. We know it is either x, t or θ
  assert(symbol.isUserVariable());
  if (SymbolHelper::IsUserVariable(symbol, k_cartesianSymbol)) {
    return k_cartesianSymbol;
  }
  if (SymbolHelper::IsUserVariable(symbol, k_polarSymbol)) {
    return k_polarSymbol;
  }
  assert(SymbolHelper::IsUserVariable(symbol, k_parametricSymbol));
  return k_parametricSymbol;
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::Model::setContent(
    Ion::Storage::Record* record, const Layout& l,
    const SymbolContext& symbolContext, CodePoint symbol) {
  UserExpression e = BuildFunctionFromLayout(l, symbol, symbolContext);
  return setExpressionContent(record, e);
}

Poincare::UserExpression ContinuousFunction::Model::BuildFunctionFromLayout(
    const Poincare::Layout& l, CodePoint symbol,
    const SymbolContext& symbolContext) {
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
      UserExpression::Parse(l, symbolContext, {.isAssignment = true});
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
    if (expressionToStore.deepSearch(SymbolHelper::IsTheta)) {
      symbol = SymbolHelper::IsUserVariable(
                   expressionToStore.cloneChildAtIndex(0), k_polarSymbol)
                   ? k_radiusSymbol
                   : k_polarSymbol;
    }
    // Fallback on normal parsing (replace symbol with unknown)
    expressionToStore =
        ExpressionModel::BuildExpressionFromLayout(l, symbol, symbolContext);
  }

  return expressionToStore;
}

void TidyIfDownStreamOf(Expression& expression,
                        const PoolObject* treePoolCursor) {
  if (treePoolCursor == nullptr || expression.isDownstreamOf(treePoolCursor)) {
    expression = Expression();
  }
}

void ContinuousFunction::Model::tidyDownstreamPoolFrom(
    const PoolObject* treePoolCursor) const {
  TidyIfDownStreamOf(m_expressionFirstDerivateApproximated, treePoolCursor);
  TidyIfDownStreamOf(m_expressionSecondDerivateApproximated, treePoolCursor);
  TidyIfDownStreamOf(m_expressionSlope, treePoolCursor);
  TidyIfDownStreamOf(m_expressionApproximated, treePoolCursor);
  TidyIfDownStreamOf(m_expressionForAnalysis, treePoolCursor);
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

ContinuousFunctionProperties ContinuousFunction::Model::properties(
    const Ion::Storage::Record* record) const {
  if (!m_properties.isInitialized()) {
    EquationData equationData = expressionEquation(record);
    m_properties = ContinuousFunctionProperties::Compute(
        expressionReducedForAnalysis(record),
        originalEquation(record, UCodePointUnknown),
        GlobalContextAccessor::Context(), this->complexFormat(record),
        equationData.equationType, equationData.functionSymbol,
        equationData.isCartesianEquation);
  }
  assert(m_properties.isInitialized());
  return m_properties;
}

int ContinuousFunction::Model::numberOfSubCurves(
    const Ion::Storage::Record* record) const {
  ContinuousFunctionProperties prop = properties(record);
  if (prop.isCartesian()) {
    const SystemExpression& e = expressionReduced(record);
    if (e.isList()) {
      assert(prop.isOfDegreeTwo());
      return e.numberOfChildren();
    }
  }
  return 1;
}

SystemExpression ContinuousFunction::Model::parametricForm(
    const Ion::Storage::Record* record) const {
  ContinuousFunctionProperties prop = properties(record);
  assert(prop.isPolar() || prop.isInversePolar() || prop.isParametric());
  SystemExpression e = expressionReduced(record);
  if (prop.isPolar() || prop.isInversePolar()) {
    /* Turn r(θ) into f(θ) = (x(θ), y(θ)) with
     * - x(θ) = r(θ) * cos(θ)
     * - y(θ) = r(θ) * sin(θ)
     * And θ(r) into f(r) = (x(r), y(r)) with
     * - x(r) = r * cos(θ(r))
     * - y(r) = r * sin(θ(r)) */
    e = SystemExpression::CreateReduce(
        KPoint(KMult(KA, KTrig(KB, 0_e)), KMult(KA, KTrig(KB, 1_e))),
        {.KA = prop.isPolar() ? e : SymbolHelper::SystemSymbol(),
         .KB = prop.isPolar() ? SymbolHelper::SystemSymbol() : e});
  } else {
    assert(prop.isParametric());
    e = e.clone();
  }
  assert(e.isPoint());
  return e;
}

Layout ContinuousFunction::Model::layout(const Ion::Storage::Record* record,
                                         CodePoint symbol) const {
  ContinuousFunction f(*record);
  if (f.isDerivative()) {
    if (m_layout.isUninitialized()) {
      m_layout = f.derivativeTitleLayout();
    }
    return m_layout;
  }
  return ExpressionModel::layout(record, symbol);
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
  GlobalContextAccessor::ContinuousFunctionStore().setStorageChangeFlag(true);
}

template Coordinate2D<float>
ContinuousFunction::templatedApproximateAtParameter<float>(float,

                                                           int) const;
template Coordinate2D<double>
ContinuousFunction::templatedApproximateAtParameter<double>(double,

                                                            int) const;

template Coordinate2D<float>
ContinuousFunction::privateEvaluateXYAtParameter<float>(float, int) const;
template Coordinate2D<double>
ContinuousFunction::privateEvaluateXYAtParameter<double>(double, int) const;
template PointOrRealScalar<double> ContinuousFunction::approximateDerivative(
    double t, int derivationOrder, bool useDomain) const;

}  // namespace Shared
