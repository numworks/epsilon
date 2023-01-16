#include "continuous_function.h"
#include <apps/apps_container_helper.h>
#include <poincare/derivative.h>
#include <poincare/float.h>
#include <poincare/function.h>
#include <poincare/helpers.h>
#include <poincare/integral.h>
#include <poincare/matrix.h>
#include <poincare/polynomial.h>
#include <poincare/print.h>
#include <poincare/serialization_helper.h>
#include <poincare/string_layout.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/symbol_abstract.h>
#include <poincare/trigonometry.h>
#include <poincare/undefined.h>
#include <poincare/zoom.h>
#include <escher/palette.h>
#include "poincare_helpers.h"
#include <algorithm>
#include "global_context.h"

using namespace Poincare;

namespace Shared {

/* ContinuousFunction - Public */

ContinuousFunction ContinuousFunction::NewModel(Ion::Storage::Record::ErrorStatus * error, const char * baseName) {
  static int s_colorIndex = 0;
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
  Ion::Storage::Record record = Ion::Storage::Record(baseName, Ion::Storage::funcExtension);
  RecordDataBuffer data(Escher::Palette::nextDataColor(&s_colorIndex));
  *error = Ion::Storage::FileSystem::sharedFileSystem()->createRecordWithExtension(baseName, Ion::Storage::funcExtension, &data, sizeof(data));
  // Return the ContinuousFunction with the new record
  return ContinuousFunction(*error == Ion::Storage::Record::ErrorStatus::None ? record : Record());
}

ContinuousFunction::ContinuousFunction(Ion::Storage::Record record) : Function(record), m_cache(nullptr) {
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
    Ion::Storage::Record::ErrorStatus error = updateNameIfNeeded(AppsContainerHelper::sharedAppsContainerGlobalContext());
    assert(error == Ion::Storage::Record::ErrorStatus::None);
    (void)error;
  }
}

ContinuousFunctionProperties ContinuousFunction::properties() const {
  if (!m_model.properties().isInitialized()) {
    // Computing the expression equation will update the function properties
    expressionReducedForAnalysis(AppsContainerHelper::sharedAppsContainerGlobalContext());
  }
  assert(m_model.properties().isInitialized());
  return m_model.properties();
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::updateNameIfNeeded(Context * context) {
  return m_model.renameRecordIfNeeded(this, context);
}

int ContinuousFunction::nameWithArgument(char * buffer, size_t bufferSize) {
  if (isNamed()) {
    return Function::nameWithArgument(buffer, bufferSize);
  }
  return UTF8Decoder::CodePointToCharsWithNullTermination(properties().isPolar() ? k_radiusSymbol : k_ordinateSymbol, buffer, bufferSize);
}

int ContinuousFunction::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Context * context, bool symbolValue) {
  ContinuousFunctionProperties thisProperties = properties();

  if (symbolValue) {
    /* With Vertical curves, cursorT != cursorX .
     * We need the value for symbol=... */
    return PoincareHelpers::ConvertFloatToText<double>(thisProperties.isCartesian() ? cursorX : cursorT, buffer, bufferSize, precision);
  }


  if (thisProperties.isParametric()) {
    Preferences::PrintFloatMode mode = Poincare::Preferences::sharedPreferences()->displayMode();
    return Poincare::Print::CustomPrintf(buffer, bufferSize, "(%*.*ed;%*.*ed)", cursorX, mode, precision, cursorY, mode, precision);
  }
  if (thisProperties.isPolar()) {
    return PoincareHelpers::ConvertFloatToText<double>(evaluate2DAtParameter(cursorT, context).x2(), buffer, bufferSize, precision);
  }
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer, bufferSize, precision);
}

Poincare::Layout ContinuousFunction::titleLayout(Poincare::Context * context, bool shortVersion) {
  if (shortVersion) {
    constexpr size_t bufferNameSize = k_maxNameWithArgumentSize + 1;
    char buffer[bufferNameSize];
    nameWithArgument(buffer, bufferNameSize);
    return StringLayout::Builder(buffer);
  }
  return PoincareHelpers::CreateLayout(originalEquation(), context);
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::setContent(const char * c, Context * context) {
  setCache(nullptr);
  bool wasCartesian = properties().isCartesian();
  /* About to set the content, the symbol does not matter here yet. We don't use
   * ExpressionModelHandle::setContent implementation to avoid calling symbol()
   * and any unnecessary plot type update at this point. See comment in
   * ContinuousFunction::Model::buildExpressionFromText. */
  Ion::Storage::Record::ErrorStatus error = editableModel()->setContent(this, c, context, k_unnamedExpressionSymbol);
  if (error == Ion::Storage::Record::ErrorStatus::None && !isNull()) {
    // Set proper name
    error = updateNameIfNeeded(context);
    // Update model
    updateModel(context, wasCartesian);
  }
  return error;
}

void ContinuousFunction::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  ExpressionModelHandle::tidyDownstreamPoolFrom(treePoolCursor);
  m_cache = nullptr;
}

bool ContinuousFunction::isNamed() const {
  // Unnamed functions have a fullname starting with k_unnamedRecordFirstChar
  const char * recordFullName = fullName();
  return recordFullName != nullptr && recordFullName[0] != k_unnamedRecordFirstChar;
}

bool ContinuousFunction::isDiscontinuousBetweenFloatValues(float x1, float x2, Poincare::Context * context) const {
  Expression equation = expressionReduced(context);
  return equation.isDiscontinuousBetweenValuesForSymbol(k_unknownName, x1, x2, context, complexFormat(context), Poincare::Preferences::sharedPreferences()->angleUnit());
}

void ContinuousFunction::getLineParameters(double * slope, double * intercept, Context * context) const {
  assert(properties().isLine());
  Expression equation = expressionReduced(context);
  // Compute metrics for details view of Line
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Separate the two line coefficients for approximation.
  int d = equation.getPolynomialReducedCoefficients(
      k_unknownName, coefficients, context, complexFormat(context), Poincare::Preferences::sharedPreferences()->angleUnit(),
      ContinuousFunctionProperties::k_defaultUnitFormat,
      ExpressionNode::SymbolicComputation::
          ReplaceAllSymbolsWithDefinitionsOrUndefined);
  assert(d <= 1);
  // Degree might vary depending on symbols definition and complex format.
  // Approximate and return the two line coefficients
  if (d < 0) {
    *slope = NAN;
    *intercept = NAN;
  } else {
    *intercept = coefficients[0].approximateToScalar<double>(context, complexFormat(context), Poincare::Preferences::sharedPreferences()->angleUnit());
    if (d == 0) {
      *slope = 0.0;
    } else {
      *slope = coefficients[1].approximateToScalar<double>(context, complexFormat(context), Poincare::Preferences::sharedPreferences()->angleUnit());
    }
  }
}

CartesianConic ContinuousFunction::cartesianConicParameters(Context * context) const {
  assert(properties().isConic() && properties().isCartesian());
  return CartesianConic(expressionReducedForAnalysis(context), context, k_unknownName);
}

double ContinuousFunction::evaluateCurveParameter(int index, double cursorT, double cursorX, double cursorY, Context * context) const {
  switch (properties().symbolType()) {
  case ContinuousFunctionProperties::SymbolType::T:
    return index == 0 ? cursorT : index == 1 ? evaluateXYAtParameter(cursorT, context).x1() : evaluateXYAtParameter(cursorT, context).x2();
  case ContinuousFunctionProperties::SymbolType::Theta:
    return index == 0 ? cursorT : evaluate2DAtParameter(cursorT, context).x2();
  default:
    return index == 0 ? cursorX : cursorY;
  }
}

void ContinuousFunction::updateModel(Context * context, bool wasCartesian) {
  setCache(nullptr);
  m_model.resetProperties(); // Reset model's properties.
  properties(); // update properties.
  assert(m_model.properties().isInitialized());
  if (wasCartesian != properties().isCartesian() || !properties().canHaveCustomDomain()) {
    // The definition's domain must be reset.
    setTAuto(true);
  }
}

int ContinuousFunction::derivativeNameWithArgument(char * buffer, size_t bufferSize) {
  if (!isNamed()) {
    return strlcpy(buffer, "y'", bufferSize);
  }
  const CodePoint derivative = '\'';
  int derivativeSize = UTF8Decoder::CharSizeOfCodePoint(derivative);
  // Fill buffer with f(x). Keep size for derivative sign.
  int numberOfChars = nameWithArgument(buffer, bufferSize - derivativeSize);
  assert(numberOfChars + derivativeSize < (int)bufferSize);
  // Find (x)
  char * firstParenthesis = const_cast<char *>(UTF8Helper::CodePointSearch(buffer, '('));
  assert(UTF8Helper::CodePointIs(firstParenthesis, '('));
  // Move parentheses to fit derivative CodePoint
  memmove(firstParenthesis + derivativeSize, firstParenthesis, numberOfChars - (firstParenthesis - buffer) + 1);
  // Insert derivative CodePoint
  UTF8Decoder::CodePointToChars(derivative, firstParenthesis, derivativeSize);
  return numberOfChars + derivativeSize;
}

double ContinuousFunction::approximateDerivative(double x, Context * context, int subCurveIndex, bool useDomain) const {
  assert(canDisplayDerivative());
  assert(subCurveIndex < numberOfSubCurves());
  if ((useDomain && (x < tMin() || x > tMax())) || isAlongY() || numberOfSubCurves() > 1) {
    return NAN;
  }
  // Derivative is simplified once and for all
  Expression derivate = expressionDerivateReduced(context);
  assert(subCurveIndex == 0);
  Preferences preferences = Preferences::ClonePreferencesWithNewComplexFormat(complexFormat(context));
  return PoincareHelpers::ApproximateWithValueForSymbol(derivate, k_unknownName, x, context, &preferences, false);
}

Poincare::Layout ContinuousFunction::derivativeTitleLayout() {
  constexpr size_t bufferNameSize = ContinuousFunction::k_maxNameWithArgumentSize + 1;
  char buffer[bufferNameSize];
  derivativeNameWithArgument(buffer, bufferNameSize);
  return StringLayout::Builder(buffer);
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
  return properties().isCartesian() ? INFINITY : 2.0 * Trigonometry::PiInAngleUnit(Preferences::sharedPreferences()->angleUnit());
}

float ContinuousFunction::autoTMin() const {
  return properties().isCartesian() ? -INFINITY : 0.0;
}

bool ContinuousFunction::basedOnCostlyAlgorithms(Context * context) const {
  return expressionReduced(context).recursivelyMatches(
    [](const Expression e, Context * context) {
      return !e.isUninitialized()
             && (e.type() == ExpressionNode::Type::Sequence
                || e.type() == ExpressionNode::Type::Integral
                || e.type() == ExpressionNode::Type::Derivative);
    });
}

void ContinuousFunction::trimResolutionInterval(double * start, double * end) const {
  double tmin = tMin(), tmax = tMax();
  *start = *start < tmin ? tmin : tmax < *start ? tmax : *start;
  *end = *end < tmin ? tmin : tmax < *end ? tmax : *end;
}

Expression ContinuousFunction::sumBetweenBounds(double start, double end, Context * context) const {
  assert(properties().isCartesian());
  start = std::max<double>(start, tMin());
  end = std::min<double>(end, tMax());
  return Integral::Builder(expressionReduced(context).clone(), Symbol::Builder(UCodePointUnknown), Float<double>::Builder(start), Float<double>::Builder(end)); // Integral takes ownership of args
  /* TODO: when we approximate integral, we might want to simplify the integral
   * here. However, we might want to do it once for all x (to avoid lagging in
   * the derivative table. */
}

/* ContinuousFunction - Private */

float ContinuousFunction::rangeStep() const {
  return properties().isCartesian() ? NAN : (tMax() - tMin())/k_polarParamRangeSearchNumberOfPoints;
}

template <typename T>
Coordinate2D<T> ContinuousFunction::privateEvaluateXYAtParameter(T t, Context * context, int subCurveIndex) const {
  Coordinate2D<T> x1x2 = templatedApproximateAtParameter(t, context, subCurveIndex);
  if (!properties().isPolar()) {
    return x1x2;
  }
  const T angle = x1x2.x1() * M_PI / Trigonometry::PiInAngleUnit(Poincare::Preferences::sharedPreferences()->angleUnit());
  return Coordinate2D<T>(x1x2.x2() * std::cos(angle),
                         x1x2.x2() * std::sin(angle));
}

template<typename T>
Coordinate2D<T> ContinuousFunction::templatedApproximateAtParameter(T t, Context * context, int subCurveIndex) const {
  if (t < tMin() || t > tMax()) {
    return Coordinate2D<T>(properties().isCartesian() ? t : NAN, NAN);
  }
  Expression e = expressionReduced(context);
  Preferences preferences = Preferences::ClonePreferencesWithNewComplexFormat(complexFormat(context));
  if (!properties().isParametric()) {
    if (numberOfSubCurves() >= 2) {
      assert(e.numberOfChildren() > subCurveIndex);
      e = e.childAtIndex(subCurveIndex);
    } else {
      assert(subCurveIndex == 0);
    }
    if (isAlongY()) {
      // Invert x and y with vertical lines so it can be scrolled vertically
      return Coordinate2D<T>(PoincareHelpers::ApproximateWithValueForSymbol(e, k_unknownName, t, context, &preferences, false), t);
    }
    return Coordinate2D<T>(t, PoincareHelpers::ApproximateWithValueForSymbol(e, k_unknownName, t, context, &preferences, false));
  }
  if (e.type() == ExpressionNode::Type::Dependency) {
    e = e.childAtIndex(0);
  }
  if (e.isUndefined()) {
    return Coordinate2D<T>(NAN, NAN);
  }
  // TODO : This should maybe be a List instead of a Matrix
  assert(e.type() == ExpressionNode::Type::Matrix);
  assert(static_cast<Matrix&>(e).numberOfRows() == 2);
  assert(static_cast<Matrix&>(e).numberOfColumns() == 1);
  return Coordinate2D<T>(
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(0), k_unknownName, t, context, &preferences, false),
      PoincareHelpers::ApproximateWithValueForSymbol(e.childAtIndex(1), k_unknownName, t, context, &preferences, false));
}

/* ContinuousFunction::Model */

Expression ContinuousFunction::Model::expressionReduced(const Ion::Storage::Record * record, Context * context) const {
  // m_expression might already be memmoized.
  if (m_expression.isUninitialized()) {
    // Retrieve the expression equation's expression.
    m_expression = expressionReducedForAnalysis(record, context);
    if (properties().status() != ContinuousFunctionProperties::Status::Enabled) {
      m_expression = Undefined::Builder();
      return m_expression;
    }
    Preferences preferences = Preferences::ClonePreferencesWithNewComplexFormat(complexFormat(record, context));
    if (!properties().isPolar() && (record->fullName() == nullptr || record->fullName()[0] == k_unnamedRecordFirstChar)) {
      /* Function isn't named, m_expression currently is an expression in y or x
       * such as y = x. We extract the solution by solving in y or x. */
      int yDegree = m_expression.polynomialDegree(context, ContinuousFunctionProperties::k_ordinateName);
      bool willBeAlongX = true;
      if (yDegree < 1 || yDegree > 2) {
        int xDegree = m_expression.polynomialDegree(context, k_unknownName);
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
      Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
      int degree = m_expression.getPolynomialReducedCoefficients(
          willBeAlongX ? ContinuousFunctionProperties::k_ordinateName : k_unknownName, coefficients,
          context,
          preferences.complexFormat(),
          preferences.angleUnit(),
          ContinuousFunctionProperties::k_defaultUnitFormat,
          ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
          true);
      assert(!willBeAlongX || degree == yDegree);
      ExpressionNode::ReductionContext reductionContext(context, preferences.complexFormat(), preferences.angleUnit(), Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::SystemForAnalysis);
      if (degree == 1) {
        Polynomial::LinearPolynomialRoots(
          coefficients[1],
          coefficients[0],
          &m_expression,
          reductionContext,
          false);
      } else if (degree == 2) {
        // Equation is of degree 2, each root is a subcurve to plot.
        assert(m_properties.isOfDegreeTwo());
        Expression root1, root2, delta;
        int solutions = Polynomial::QuadraticPolynomialRoots(
            coefficients[2],
            coefficients[1],
            coefficients[0],
            &root1,
            &root2,
            &delta,
            reductionContext,
            false,
            false);
        if (solutions <= 1) {
          m_expression = root1;
        } else {
          // SubCurves are stored in a 2x1 matrix
          Matrix newExpr = Matrix::Builder();
          // Roots are ordered so that the first one is superior to the second
          newExpr.addChildAtIndexInPlace(root2, 0, 0);
          newExpr.addChildAtIndexInPlace(root1, 1, 1);
          newExpr.setDimensions(2, 1);
          m_expression = newExpr;
        }
      } else {
        /* TODO : We could handle simple equations of any degree by solving the
         * equation within the graph view bounds, to plot as many vertical or
         * horizontal lines as needed. */
        m_expression = Undefined::Builder();
        return m_expression;
      }
      if (!willBeAlongX && yDegree != 0) {
        // No need to replace anything if yDegree is 0
        m_expression.replaceSymbolWithExpression(Symbol::Builder(k_ordinateSymbol), Symbol::Builder(UCodePointUnknown));
      }
    } else {
      /* m_expression is resulting of a simplification with the target
      * SystemForAnalysis. But SystemForApproximation can be
      * sometimes way simpler than the one from SystemForAnalysis.
      * For example (x+9)^6 is fully developped in SystemForAnalysis,
      * which results in approximation inaccuracy.
      * On the other hand, the expression (x+1)^2-x^2-2x-1 should be developped
      * so that we understand that it's equal to zero, and is better handled
      * by SystemForAnalysis.
      * To solve this problem, we try to simplify both ways and compare the number
      * of nodes of each expression. We take the one that has the less node.
      * This is not ideal because an expression with less node does not always
      * mean a simpler expression, but it's a good compromise for now. */
      Expression resultForApproximation = expressionEquation(record, context);
      if (!resultForApproximation.isUninitialized()) {
        PoincareHelpers::CloneAndReduce(
            &resultForApproximation,
            context,
            ExpressionNode::ReductionTarget::SystemForApproximation,
            ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol,
            PoincareHelpers::k_defaultUnitConversion,
            &preferences, false);
        if (resultForApproximation.numberOfDescendants(true) < m_expression.numberOfDescendants(true)) {
          m_expression = resultForApproximation;
        }
      }
    }
  }
  return m_expression;
}

Poincare::Expression ContinuousFunction::Model::expressionReducedForAnalysis(const Ion::Storage::Record * record, Poincare::Context * context) const {
  ContinuousFunctionProperties::SymbolType computedFunctionSymbol = ContinuousFunctionProperties::k_defaultSymbolType;
  ComparisonNode::OperatorType computedEquationType = ContinuousFunctionProperties::k_defaultEquationType;
  bool isCartesianEquation = false;
  Expression result = expressionEquation(record, context, &computedEquationType, &computedFunctionSymbol, &isCartesianEquation);
  if (!result.isUndefined()) {
    Preferences preferences = Preferences::ClonePreferencesWithNewComplexFormat(complexFormat(record, context));
    PoincareHelpers::CloneAndReduce(
        &result,
        context,
        ExpressionNode::ReductionTarget::SystemForAnalysis,
        ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol, // Symbols have already been replaced.
        PoincareHelpers::k_defaultUnitConversion,
        &preferences, false);
  }
  if (!m_properties.isInitialized()) {
    // Use the computed equation to update the plot type.
    m_properties.update(result, originalEquation(record, UCodePointUnknown), context, computedEquationType, computedFunctionSymbol, isCartesianEquation);
  }
  return result;
}

Expression ContinuousFunction::Model::expressionClone(const Ion::Storage::Record * record) const {
  assert(record->fullName() != nullptr && record->fullName()[0] != k_unnamedRecordFirstChar);
  Expression e = ExpressionModel::expressionClone(record);
  if (e.isUninitialized()) {
    return e;
  }
  return e.childAtIndex(1);
}

Expression ContinuousFunction::Model::originalEquation(const Ion::Storage::Record * record, CodePoint symbol) const {
  Expression unknownSymbolEquation = ExpressionModel::expressionClone(record);
  if (unknownSymbolEquation.isUninitialized() || symbol == UCodePointUnknown) {
    return unknownSymbolEquation;
  }
  return unknownSymbolEquation.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
}

bool isValidNamedLeftExpression(const Expression e, ComparisonNode::OperatorType equationType) {
  /* Examples of valid named expression : f(x)= or f(x)< or f(t)=
   * Examples of invalid named expression : cos(x)= or f(θ)< or f(t)<  */
  if (e.type() != ExpressionNode::Type::Function || equationType == ComparisonNode::OperatorType::NotEqual) {
    return false;
  }
  Expression functionSymbol = e.childAtIndex(0);
  return functionSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_cartesianSymbol))
         || (equationType == ComparisonNode::OperatorType::Equal
             && functionSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_parametricSymbol)));
}

Expression ContinuousFunction::Model::expressionEquation(const Ion::Storage::Record * record, Context * context, ComparisonNode::OperatorType * computedEquationType, ContinuousFunctionProperties::SymbolType * computedFunctionSymbol, bool * isCartesianEquation) const {
  Expression result = ExpressionModel::expressionClone(record);
  if (result.isUninitialized()) {
    return Undefined::Builder();
  }
  ContinuousFunctionProperties::SymbolType tempFunctionSymbol = ContinuousFunctionProperties::k_defaultSymbolType;
  ComparisonNode::OperatorType equationType;
  if (!ComparisonNode::IsBinaryComparison(result, &equationType) || equationType == ComparisonNode::OperatorType::NotEqual) {
    /* Happens when the input text is too long and "f(x)=" can't be inserted
     * or when inputting amiguous equations like "x+y>2>y" */
    return Undefined::Builder();
  }
 if (computedEquationType) {
    *computedEquationType = equationType;
  }
  bool isUnnamedFunction = true;
  Expression leftExpression = result.childAtIndex(0);

  if (isValidNamedLeftExpression(leftExpression, equationType)) {
    // Ensure that function name is either record's name, or free
    assert(record->fullName() != nullptr);
    assert(leftExpression.type() == Poincare::ExpressionNode::Type::Function);
    const char * functionName = static_cast<Poincare::Function&>(leftExpression).name();
    const size_t functionNameLength = strlen(functionName);
    if (Shared::GlobalContext::SymbolAbstractNameIsFree(functionName)
        || (record->fullName()[0] != k_unnamedRecordFirstChar
            && strncmp(record->name().baseName, functionName, functionNameLength) == 0)) {
      Expression functionSymbol = leftExpression.childAtIndex(0);
      // Set the model's plot type.
      if (functionSymbol.isIdenticalTo(Symbol::Builder(k_parametricSymbol))) {
        tempFunctionSymbol = ContinuousFunctionProperties::SymbolType::T;
      } else {
        assert(functionSymbol.isIdenticalTo(Symbol::Builder(k_cartesianSymbol)));
        tempFunctionSymbol = ContinuousFunctionProperties::SymbolType::X;
      }
      result = result.childAtIndex(1);
      isUnnamedFunction = false;
    } else {
      /* Function in first half of the equation refer to an already defined one.
       * Replace the symbol. */
      leftExpression.replaceChildAtIndexInPlace(0, Symbol::Builder(UCodePointUnknown));
    }
  } else if (leftExpression.isIdenticalTo(Symbol::Builder(k_radiusSymbol))) {
    result = result.childAtIndex(1);
    tempFunctionSymbol = ContinuousFunctionProperties::SymbolType::Theta;
    isUnnamedFunction = false;
  }
  if (computedFunctionSymbol) {
    *computedFunctionSymbol = tempFunctionSymbol;
  }
  if (isCartesianEquation) {
    *isCartesianEquation = isUnnamedFunction;
  }
  if (isUnnamedFunction) {
    result = Subtraction::Builder(leftExpression, result.childAtIndex(1));
    /* Replace all y symbols with UCodePointTemporaryUnknown so that they are
     * not replaced if they had a predefined value. This will not replace the y
     * symbols nested in function, which is not a supported behavior anyway.
     * TODO: Make a consistent behavior calculation/additional_outputs using a
     *       VariableContext to temporary disable y's predefinition. */
    result = result.replaceSymbolWithExpression(Symbol::Builder(k_ordinateSymbol), Symbol::Builder(UCodePointTemporaryUnknown));
  }
  // Replace all defined symbols and functions to extract symbols
  result = Expression::ExpressionWithoutSymbols(result, context);
  if (result.isUninitialized()) {
    // result was Circularly defined
    return Undefined::Builder();
  }
  if (isUnnamedFunction) {
    result = result.replaceSymbolWithExpression(Symbol::Builder(UCodePointTemporaryUnknown), Symbol::Builder(k_ordinateSymbol));
  }
  assert(!result.isUninitialized());
  return result;
}

Expression ContinuousFunction::Model::expressionDerivateReduced(const Ion::Storage::Record * record, Context * context) const {
  // m_expressionDerivate might already be memmoized.
  if (m_expressionDerivate.isUninitialized()) {
    Expression expression = expressionReduced(record, context).clone();
    // Derivative isn't available on curves with multiple subcurves
    if (numberOfSubCurves(record) > 1) {
      m_expressionDerivate = Undefined::Builder();
    } else {
      m_expressionDerivate = Derivative::Builder(expression, Symbol::Builder(UCodePointUnknown), Symbol::Builder(UCodePointUnknown));
      Preferences preferences = Preferences::ClonePreferencesWithNewComplexFormat(complexFormat(record, context));
      /* On complex functions, this step can take a significant time.
      * A workaround could be to identify big functions to skip simplification
      * at the cost of possible inaccurate evaluations (such as diff(abs(x),x,0)
      * not being undefined). */
      PoincareHelpers::CloneAndSimplify(&m_expressionDerivate, context, ExpressionNode::ReductionTarget::SystemForApproximation, ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, PoincareHelpers::k_defaultUnitConversion, &preferences, false);
    }
  }
  return m_expressionDerivate;
}

Ion::Storage::Record::ErrorStatus ContinuousFunction::Model::renameRecordIfNeeded(Ion::Storage::Record * record,  Context * context) const {
  /* Use ExpressionModel::expressionClone because it does not alter
   * the left-hand side of "f(x)=" and "f(t)=", which allows the name
   * of the function to be found. */
  Expression newExpression = ExpressionModel::expressionClone(record);
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (newExpression.isUninitialized()) {
    /* The expression is not set, the record will be named later or is already
     * correctly named by GlobalContext. */
    return error;
  }
  if (record->hasExtension(Ion::Storage::funcExtension)) {
    ComparisonNode::OperatorType newOperatorType;
    if (ComparisonNode::IsBinaryComparison(newExpression, &newOperatorType)
        && isValidNamedLeftExpression(newExpression.childAtIndex(0), newOperatorType)) {
      Expression function = newExpression.childAtIndex(0);
      error = Ion::Storage::Record::SetBaseNameWithExtension(record, static_cast<SymbolAbstract&>(function).name(), Ion::Storage::funcExtension);
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
    const char * const extensions[1] = { Ion::Storage::funcExtension };
    name[0] = k_unnamedRecordFirstChar;
    Ion::Storage::FileSystem::sharedFileSystem()->firstAvailableNameFromPrefix(name, 1, k_maxDefaultNameSize, extensions, 1, 99);
    error = Ion::Storage::Record::SetBaseNameWithExtension(record, name, Ion::Storage::funcExtension);
  }
  return error;
}

Poincare::Expression ContinuousFunction::Model::buildExpressionFromText(const char * c, CodePoint symbol, Poincare::Context * context) const {
  /* The symbol parameter is discarded in this implementation. Either there is a
   * valid named left expression and the symbol will be extracted, either the
   * symbol should be the default symbol used in unnamed expressions. */
  assert(symbol == k_unnamedExpressionSymbol);
  Expression expressionToStore;
  bool isFunctionAssignment = false;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    /* Parse the expression to store as possible function assignment. */
    expressionToStore = Expression::Parse(c, context, true, true);
    if (expressionToStore.isUninitialized()) {
      return expressionToStore;
    }
    // Check if the equation is of the form f(x)=...
    ComparisonNode::OperatorType comparisonType;
    if (ComparisonNode::IsBinaryComparison(expressionToStore, &comparisonType)
      && isValidNamedLeftExpression(expressionToStore.childAtIndex(0), comparisonType)) {
      isFunctionAssignment = true;
      Expression functionSymbol = expressionToStore.childAtIndex(0).childAtIndex(0);
      // Extract the CodePoint function's symbol. We know it is either x, t or θ
      assert(functionSymbol.type() == ExpressionNode::Type::Symbol);
      // Override the symbol so that it can be replaced in the right expression
      if (functionSymbol.isIdenticalTo(Symbol::Builder(k_cartesianSymbol))) {
        symbol = k_cartesianSymbol;
      } else {
        assert((functionSymbol.isIdenticalTo(Symbol::Builder(k_parametricSymbol))));
        symbol = k_parametricSymbol;
      }
    }
  }

  if (expressionToStore.isUninitialized()) {
    return expressionToStore;
  }

  if (isFunctionAssignment) {
    // Do not replace symbol in f(x)=
    ExpressionModel::ReplaceSymbolWithUnknown(expressionToStore.childAtIndex(1), symbol);
  } else {
    if (expressionToStore.recursivelyMatches(
      [](const Expression e, Context * context) {
        return e.type() == ExpressionNode::Type::Symbol && AliasesLists::k_thetaAliases.contains(static_cast<const Symbol &>(e).name());
      }))
    {
      symbol = k_polarSymbol;
    }
    // Fallback on normal parsing
    expressionToStore = ExpressionModel::buildExpressionFromText(c, symbol, context);
  }

  return expressionToStore;
}

void ContinuousFunction::Model::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  if (treePoolCursor == nullptr || m_expressionDerivate.isDownstreamOf(treePoolCursor)) {
    resetProperties();
    m_expressionDerivate = Expression();
  }
  ExpressionModel::tidyDownstreamPoolFrom(treePoolCursor);
}

int ContinuousFunction::Model::numberOfSubCurves(const Ion::Storage::Record * record) const {
  if (properties().isCartesian()) {
    Expression e = expressionReduced(record, AppsContainerHelper::sharedAppsContainerGlobalContext());
    if (e.type() == ExpressionNode::Type::Matrix) {
      assert(properties().isOfDegreeTwo());
      assert(static_cast<Matrix&>(e).numberOfColumns() == 1);
      // We could handle any number of subcurves and any number of rows
      assert(static_cast<Matrix&>(e).numberOfRows() == 2);
      return static_cast<Matrix&>(e).numberOfRows();
    }
  }
  return 1;
}

void * ContinuousFunction::Model::expressionAddress(const Ion::Storage::Record * record) const {
  return (char *)record->value().buffer+sizeof(RecordDataBuffer);
}

size_t ContinuousFunction::Model::expressionSize(const Ion::Storage::Record * record) const {
  return record->value().size-sizeof(RecordDataBuffer);
}


template Coordinate2D<float> ContinuousFunction::templatedApproximateAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::templatedApproximateAtParameter<double>(double, Context *, int) const;

template Coordinate2D<float> ContinuousFunction::privateEvaluateXYAtParameter<float>(float, Context *, int) const;
template Coordinate2D<double> ContinuousFunction::privateEvaluateXYAtParameter<double>(double, Context *, int) const;


} // namespace Graph
