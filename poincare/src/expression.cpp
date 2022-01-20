#include <poincare/expression.h>
#include <poincare/expression_node.h>
#include <poincare/ghost.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
#include <ion.h>
#include <ion/unicode/utf8_helper.h>
#include <cmath>
#include <float.h>
#include <utility>

#include "parsing/parser.h"

namespace Poincare {

bool Expression::sSymbolReplacementsCountLock = false;
static bool sApproximationEncounteredComplex = false;

/* Constructor & Destructor */

Expression Expression::clone() const { TreeHandle c = TreeHandle::clone(); return static_cast<Expression&>(c); }

Expression Expression::Parse(char const * string, Context * context, bool addParentheses) {
  if (string[0] == 0) {
    return Expression();
  }
  Parser p(string, context);
  Expression expression = p.parse();
  if (p.getStatus() != Parser::Status::Success) {
    expression = Expression();
  }
  if (!expression.isUninitialized() && addParentheses) {
    expression = expression.addMissingParentheses();
  }
  return expression;
}

Expression Expression::ExpressionFromAddress(const void * address, size_t size) {
  if (address == nullptr || size == 0) {
    return Expression();
  }
  // Build the Expression in the Tree Pool
  return Expression(static_cast<ExpressionNode *>(TreePool::sharedPool()->copyTreeFromAddress(address, size)));
}

/* Circuit breaker */

static Expression::CircuitBreaker sCircuitBreaker = nullptr;
static bool sSimplificationHasBeenInterrupted = false;

void Expression::SetCircuitBreaker(CircuitBreaker cb) {
  sCircuitBreaker = cb;
}

bool Expression::ShouldStopProcessing() {
  if (sCircuitBreaker == nullptr) {
    return false;
  }
  if (sCircuitBreaker()) {
    sSimplificationHasBeenInterrupted = true;
    return true;
  }
  return false;
}

void Expression::SetInterruption(bool interrupt) {
  sSimplificationHasBeenInterrupted = interrupt;
}

/* Hierarchy */

Expression Expression::childAtIndex(int i) const {
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Expression &>(c);
}

/* Properties */

bool Expression::isRationalOne() const {
  return type() == ExpressionNode::Type::Rational && convert<const Rational>().isOne();
}

bool Expression::recursivelyMatches(ExpressionTest test, Context * context, ExpressionNode::SymbolicComputation replaceSymbols) const {
  if (test(*this, context)) {
    return true;
  }

  // Handle symbols and functions
  ExpressionNode::Type t = type();
  if (t == ExpressionNode::Type::Symbol || t == ExpressionNode::Type::Function) {
    assert(replaceSymbols == ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
        || replaceSymbols == ExpressionNode::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
        || replaceSymbols == ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol); // We need only those cases for now

    if (replaceSymbols == ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol
        || (replaceSymbols == ExpressionNode::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          && t == ExpressionNode::Type::Symbol))
    {
      return false;
    }
    assert(replaceSymbols == ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
        || t == ExpressionNode::Type::Function);
      return SymbolAbstract::matches(convert<const SymbolAbstract>(), test, context);
  }

  const int childrenCount = this->numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndex(i).recursivelyMatches(test, context, replaceSymbols)) {
      return true;
    }
  }
  return false;
}

bool Expression::hasExpression(ExpressionTypeTest test, const void * context) const {
  if (test(*this, context)) {
    return true;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndex(i).hasExpression(test, context)) {
      return true;
    }
  }
  return false;
}

bool Expression::deepIsMatrix(Context * context) const {
  /* We could do something virtual instead of implementing a disjunction on
   * types but in a first try, it was easier to group all code regarding
   * isMatrix at the same place. */
  if (IsMatrix(*this, context)) {
    return true;
  }
  // Scalar expressions
  ExpressionNode::Type types1[] = {ExpressionNode::Type::BinomialCoefficient, ExpressionNode::Type::Derivative, ExpressionNode::Type::Determinant, ExpressionNode::Type::DivisionQuotient, ExpressionNode::Type::DivisionRemainder, ExpressionNode::Type::Factor, ExpressionNode::Type::GreatCommonDivisor, ExpressionNode::Type::Integral, ExpressionNode::Type::LeastCommonMultiple, ExpressionNode::Type::MatrixTrace, ExpressionNode::Type::NthRoot, ExpressionNode::Type::PermuteCoefficient, ExpressionNode::Type::Randint, ExpressionNode::Type::Round, ExpressionNode::Type::SignFunction, ExpressionNode::Type::SquareRoot};
  if (isOfType(types1, sizeof(types1)/sizeof(ExpressionNode::Type))) {
    return false;
  }
  // The children were sorted so any expression which is a matrix (deeply) would be at the end
  if (IsNAry(*this, context)) {
    int nbOfChildren = numberOfChildren();
    assert(nbOfChildren > 0);
    return childAtIndex(nbOfChildren-1).deepIsMatrix(context);
  }
  // Logarithm, Power, Product, Sum are matrices only if their first child is a matrix
  ExpressionNode::Type types2[] = {ExpressionNode::Type::Logarithm, ExpressionNode::Type::Power, ExpressionNode::Type::Product, ExpressionNode::Type::Sum};
  if (isOfType(types2, sizeof(types2)/sizeof(ExpressionNode::Type))) {
    assert(numberOfChildren() > 0);
    return childAtIndex(0).deepIsMatrix(context);
  }
  // By default, an expression is a matrix of any of its children is one (eg, Cosine, Decimal...)
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndex(i).deepIsMatrix(context)) {
      return true;
    }
  }
  return false;
}

bool Expression::IsApproximate(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Decimal || e.type() == ExpressionNode::Type::Float || e.type() == ExpressionNode::Type::Double;
}

bool Expression::IsRandom(const Expression e, Context * context) {
  return e.isRandom();
}

bool Expression::IsNAry(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Addition || e.type() == ExpressionNode::Type::Multiplication;
}

bool Expression::IsMatrix(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Matrix
    || e.type() == ExpressionNode::Type::ConfidenceInterval
    || e.type() == ExpressionNode::Type::MatrixDimension
    || e.type() == ExpressionNode::Type::PredictionInterval
    || e.type() == ExpressionNode::Type::MatrixInverse
    || e.type() == ExpressionNode::Type::MatrixIdentity
    || e.type() == ExpressionNode::Type::MatrixTranspose
    || e.type() == ExpressionNode::Type::MatrixRowEchelonForm
    || e.type() == ExpressionNode::Type::MatrixReducedRowEchelonForm
    || e.type() == ExpressionNode::Type::VectorCross;
}

bool Expression::IsInfinity(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Infinity;
}

bool containsVariables(const Expression e, char * variables, int maxVariableSize) {
  if (e.type() == ExpressionNode::Type::Symbol) {
    int index = 0;
    while (variables[index*maxVariableSize] != 0) {
      if (strcmp(static_cast<const Symbol&>(e).name(), &variables[index*maxVariableSize]) == 0) {
        return true;
      }
      index++;
    }
  }
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (containsVariables(e.childAtIndex(i), variables, maxVariableSize)) {
      return true;
    }
  }
  return false;
}

bool Expression::getLinearCoefficients(char * variables, int maxVariableSize, Expression coefficients[], Expression constant[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) const {
  assert(!recursivelyMatches(IsMatrix, context, symbolicComputation));
  // variables is in fact of type char[k_maxNumberOfVariables][maxVariableSize]
  int index = 0;
  while (variables[index*maxVariableSize] != 0) {
    int degree = polynomialDegree(context, &variables[index*maxVariableSize]);
    if (degree > 1 || degree < 0) {
      return false;
    }
    index++;
  }
  Expression equation = *this;
  index = 0;
  Expression polynomialCoefficients[k_maxNumberOfPolynomialCoefficients];
  while (variables[index*maxVariableSize] != 0) {
    int degree = equation.getPolynomialReducedCoefficients(&variables[index*maxVariableSize], polynomialCoefficients, context, complexFormat, angleUnit, unitFormat, symbolicComputation);
    switch (degree) {
      case 0:
        coefficients[index] = Rational::Builder(0);
        break;
      case 1:
        coefficients[index] = polynomialCoefficients[1];
        break;
      default:
        /* degree is supposed to be 0 or 1. Otherwise, it means that equation
         * is 'undefined' due to the reduction of 0*inf for example.
         * (ie, x*y*inf = 0) */
        assert(!recursivelyMatches([](const Expression e, Context * context) { return e.isUndefined(); }, context));
        return false;
    }
    /* The equation is can be written: a_1*x+a_0 with a_1 and a_0 x-independent.
     * The equation supposed to be linear in all variables, so we can look for
     * the coefficients linked to the other variables in a_0. */
    equation = polynomialCoefficients[0];
    index++;
  }
  constant[0] = Opposite::Builder(equation.clone()).reduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation, symbolicComputation));
  /* The expression can be linear on all coefficients taken one by one but
   * non-linear (ex: xy = 2). We delete the results and return false if one of
   * the coefficients contains a variable. */
  bool isMultivariablePolynomial = containsVariables(constant[0], variables, maxVariableSize);
  for (int i = 0; i < index; i++) {
    if (isMultivariablePolynomial) {
      break;
    }
    isMultivariablePolynomial |= containsVariables(coefficients[i], variables, maxVariableSize);
  }
  return !isMultivariablePolynomial;
}

bool Expression::isDefinedCosineOrSine(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  ExpressionNode::Type t = type();
  if (t == ExpressionNode::Type::Cosine || t == ExpressionNode::Type::Sine) {
    float r = childAtIndex(0).approximateToScalar<float>(context, complexFormat, angleUnit);
    if (!std::isnan(r)) {
      return true;
    }
  }
  return false;
}

bool Expression::isBasedIntegerCappedBy(const char * stringInteger) const {
  return type() == ExpressionNode::Type::BasedInteger && (Integer::NaturalOrder(convert<BasedInteger>().integer(), Integer(stringInteger)) < 0);
}

bool Expression::isDivisionOfIntegers() const {
  return type() == ExpressionNode::Type::Division && childAtIndex(0).type() == ExpressionNode::Type::BasedInteger && childAtIndex(1).type() == ExpressionNode::Type::BasedInteger;
}

bool Expression::hasDefinedComplexApproximation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (complexFormat == Preferences::ComplexFormat::Real) {
    return false;
  }
  /* We return true when both real and imaginary approximation are defined and
   * imaginary part is not null. */
  Expression e = clone();
  Expression imag = ImaginaryPart::Builder(e);
  float b = imag.approximateToScalar<float>(context, complexFormat, angleUnit);
  if (b == 0.0f || std::isinf(b) || std::isnan(b)) {
    return false;
  }
  Expression real = RealPart::Builder(e);
  float a = real.approximateToScalar<float>(context, complexFormat, angleUnit);
  if (std::isinf(a) || std::isnan(a)) {
    return false;
  }
  return true;
}

// Private

void Expression::shallowAddMissingParenthesis() {
  if (isUninitialized()) {
    return;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression child = childAtIndex(i);
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}

Expression Expression::addMissingParentheses() {
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression child = childAtIndex(i).addMissingParentheses();
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      child = Parenthesis::Builder(child);
    }
    replaceChildAtIndexInPlace(i, child);
  }
  return *this;
}

void Expression::defaultDeepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    assert(childrenCount == numberOfChildren());
    childAtIndex(i).deepReduce(reductionContext);
  }
}

Expression Expression::defaultShallowReduce() {
  Expression result;
  if (sSimplificationHasBeenInterrupted) {
    result = Undefined::Builder();
  } else {
    const int childrenCount = numberOfChildren();
    for (int i = 0; i < childrenCount; i++) {
      /* The reduction is shortcut if one child is unreal or undefined:
       * - the result is unreal if at least one child is unreal
       * - the result is undefined if at least one child is undefined but no child
       *   is unreal */
      ExpressionNode::Type childIType = childAtIndex(i).type();
      if (childIType == ExpressionNode::Type::Unreal) {
        result = Unreal::Builder();
        break;
      } else if (childIType == ExpressionNode::Type::Undefined) {
        result = Undefined::Builder();
      }
    }
  }
  if (!result.isUninitialized()) {
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

Expression Expression::defaultHandleUnitsInChildren() {
  // Generically, an Expression does not accept any Unit in its children.
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression unit;
    Expression childI = childAtIndex(i).removeUnit(&unit);
    if (childI.isUndefined() || !unit.isUninitialized()) {
      return replaceWithUndefinedInPlace();
    }
  }
  return *this;
}

Expression Expression::shallowReduceUsingApproximation(ExpressionNode::ReductionContext reductionContext) {
  double approx = node()->approximate(double(), ExpressionNode::ApproximationContext(reductionContext, true)).toScalar();
  /* If approx is capped by the largest integer such as all smaller integers can
   * be exactly represented in IEEE754, approx is the exact result (no
   * precision were loss). */
  if (!std::isnan(approx) && std::fabs(approx) <= k_largestExactIEEE754Integer) {
    Expression result = Decimal::Builder(approx).shallowReduce();
    assert(result.type() == ExpressionNode::Type::Rational);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

void Expression::defaultDeepBeautifyChildren(ExpressionNode::ReductionContext reductionContext) {
  const int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    Expression child = childAtIndex(i);
    child = child.deepBeautify(reductionContext);
    // We add missing Parentheses after beautifying the parent and child
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}

bool Expression::SimplificationHasBeenInterrupted() {
  return sSimplificationHasBeenInterrupted;
}

Expression Expression::parent() const {
  TreeHandle p = TreeHandle::parent();
  return static_cast<Expression &>(p);
}

Expression Expression::replaceWithUndefinedInPlace() {
  Expression result = Undefined::Builder();
  replaceWithInPlace(result);
  return result;
}

void Expression::defaultSetChildrenInPlace(Expression other) {
  const int childrenCount = numberOfChildren();
  assert(childrenCount == other.numberOfChildren());
  for (int i = 0; i < childrenCount; i++) {
    replaceChildAtIndexInPlace(i, other.childAtIndex(i));
  }
}

Expression Expression::defaultReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    Expression c = childAtIndex(i).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
    if (c.isUninitialized()) { // the expression is circularly defined, escape
      return Expression();
    }
  }
  return *this;
}

Expression Expression::makePositiveAnyNegativeNumeralFactor(ExpressionNode::ReductionContext reductionContext) {
  // The expression is a negative number
  if (isNumber() && sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
    return setSign(ExpressionNode::Sign::Positive, reductionContext);
  }
  // The expression is a multiplication whose numeral factor is negative
  if (type() == ExpressionNode::Type::Multiplication && numberOfChildren() > 0 && childAtIndex(0).isNumber() && childAtIndex(0).sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
    Multiplication m = convert<Multiplication>();
    if (m.childAtIndex(0).type() == ExpressionNode::Type::Rational && m.childAtIndex(0).convert<Rational>().isMinusOne()) {
      // The negative numeral factor is -1, we just remove it
      m.removeChildAtIndexInPlace(0);
      // The multiplication can have only one child after removing -1
      return m.squashUnaryHierarchyInPlace();
    } else {
      // Otherwise, we make it positive
      m.childAtIndex(0).setSign(ExpressionNode::Sign::Positive, reductionContext);
    }
    return std::move(m);
  }
  return Expression();
}

template<typename U>
Evaluation<U> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const {
  sApproximationEncounteredComplex = false;
  // Reset interrupting flag because some evaluation methods use it
  sSimplificationHasBeenInterrupted = false;
  Evaluation<U> e = node()->approximate(U(), ExpressionNode::ApproximationContext(context, complexFormat, angleUnit, withinReduce));
  if (complexFormat == Preferences::ComplexFormat::Real && sApproximationEncounteredComplex) {
    e = Complex<U>::Undefined();
  }
  return e;
}

Expression Expression::defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression) {
  /* In this case, replacing a symbol does not alter the number of children,
   * since no other operation (e.g. reduction) is applied. */
  const int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    assert(nbChildren == numberOfChildren());
    childAtIndex(i).replaceSymbolWithExpression(symbol, expression);
  }
  return *this;
}

int Expression::defaultGetPolynomialCoefficients(Context * context, const char * symbol, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbol);
  if (deg == 0) {
    coefficients[0] = clone();
    return 0;
  }
  return -1;
}

int Expression::getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) const {
  // Reset interrupting flag because we use deepReduce
  int degree = getPolynomialCoefficients(context, symbolName, coefficients, symbolicComputation);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].reduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation, symbolicComputation));
  }
  return degree;
}

/* Units */

bool Expression::hasUnit() const {
  return recursivelyMatches([](const Expression e, Context * context) { return e.type() == ExpressionNode::Type::Unit; }, nullptr, ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol);
}

/* Complex */

bool Expression::EncounteredComplex() {
  return sApproximationEncounteredComplex;
}

void Expression::SetEncounteredComplex(bool encounterComplex) {
  sApproximationEncounteredComplex = encounterComplex;
}

Preferences::ComplexFormat Expression::UpdatedComplexFormatWithTextInput(Preferences::ComplexFormat complexFormat, const char * textInput) {
  if (complexFormat == Preferences::ComplexFormat::Real && UTF8Helper::HasCodePoint(textInput, UCodePointMathematicalBoldSmallI)) {
    return Preferences::ComplexFormat::Cartesian;
  }
  return complexFormat;
}

Preferences::ComplexFormat Expression::UpdatedComplexFormatWithExpressionInput(Preferences::ComplexFormat complexFormat, const Expression & exp, Context * context) {
  if (complexFormat == Preferences::ComplexFormat::Real && exp.recursivelyMatches([](const Expression e, Context * context) { return e.type() == ExpressionNode::Type::Constant && static_cast<const Constant &>(e).isIComplex(); }, context)) {
    return Preferences::ComplexFormat::Cartesian;
  }
  return complexFormat;
}

bool Expression::isReal(Context * context) const {
  /* We could do something virtual instead of implementing a disjunction on
   * types but many expressions have the same implementation so it is easier to
   * factorize it here. */

  // These expressions are real if their children are
  ExpressionNode::Type types1[] = {ExpressionNode::Type::ArcTangent, ExpressionNode::Type::Conjugate, ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine, ExpressionNode::Type::Tangent};
  if (isOfType(types1, sizeof(types1)/sizeof(ExpressionNode::Type))) {
    return childAtIndex(0).isReal(context);
  }

  // These expressions are always real
  ExpressionNode::Type types2[] = {ExpressionNode::Type::BinomialCoefficient, ExpressionNode::Type::Derivative, ExpressionNode::Type::DivisionQuotient, ExpressionNode::Type::DivisionRemainder, ExpressionNode::Type::GreatCommonDivisor, ExpressionNode::Type::Integral, ExpressionNode::Type::LeastCommonMultiple, ExpressionNode::Type::PermuteCoefficient, ExpressionNode::Type::Randint, ExpressionNode::Type::Random, ExpressionNode::Type::Round, ExpressionNode::Type::SignFunction, ExpressionNode::Type::Unit};
  if ((isNumber() && !isUndefined()) || isOfType(types2, sizeof(types2)/sizeof(ExpressionNode::Type))) {
    return true;
  }

  // These expressions are real when they are scalar
  ExpressionNode::Type types3[] = {ExpressionNode::Type::AbsoluteValue, ExpressionNode::Type::Ceiling, ExpressionNode::Type::ComplexArgument, ExpressionNode::Type::Factorial, ExpressionNode::Type::Floor, ExpressionNode::Type::FracPart, ExpressionNode::Type::ImaginaryPart, ExpressionNode::Type::RealPart};
  if (isOfType(types3, sizeof(types3)/sizeof(ExpressionNode::Type))) {
    return !deepIsMatrix(context);
  }

  // NAryExpresions are real if all children are real
  if (IsNAry(*this, context)) {
    return convert<NAryExpression>().allChildrenAreReal(context) == 1;
  }

  if (type() == ExpressionNode::Type::Constant) {
    return static_cast<ConstantNode *>(node())->isReal();
  }

  if (type() == ExpressionNode::Type::Power) {
    return static_cast<PowerNode *>(node())->isReal(context);
  }

  return false;
}

/* Comparison */

bool Expression::isIdenticalTo(const Expression e) const {
  /* We use the simplification order only because it is a already-coded total
   * order on expressions. */
  return ExpressionNode::SimplificationOrder(node(), e.node(), true, true) == 0;
}

bool Expression::isIdenticalToWithoutParentheses(const Expression e) const {
  // Same as isIdenticalTo, but ignoring the parentheses.
  return ExpressionNode::SimplificationOrder(node(), e.node(), true, true, true) == 0;
}

bool Expression::ParsedExpressionsAreEqual(const char * e0, const char * e1, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat) {
  Expression exp0 = Expression::ParseAndSimplify(e0, context, complexFormat, angleUnit, unitFormat, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  Expression exp1 = Expression::ParseAndSimplify(e1, context, complexFormat, angleUnit, unitFormat, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  if (exp0.isUninitialized() || exp1.isUninitialized()) {
    return false;
  }
  return exp0.isIdenticalTo(exp1);
}

/* Layout Helper */

Layout Expression::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return isUninitialized() ? Layout() : node()->createLayout(floatDisplayMode, numberOfSignificantDigits);
}

int Expression::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return isUninitialized() ? 0 : node()->serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits); }

/* Simplification */

Expression Expression::ParseAndSimplify(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation, ExpressionNode::UnitConversion unitConversion) {
  Expression exp = Parse(text, context, false);
  if (exp.isUninitialized()) {
    return Undefined::Builder();
  }
  exp = exp.simplify(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::User, symbolicComputation, unitConversion));
  /* simplify might have been interrupted, in which case the resulting
   * expression is uninitialized, so we need to check that. */
  if (exp.isUninitialized()) {
    return Parse(text, context);
  }
  return exp;
}

void Expression::ParseAndSimplifyAndApproximate(const char * text, Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation, ExpressionNode::UnitConversion unitConversion) {
  assert(simplifiedExpression);
  Expression exp = Parse(text, context, false);
  if (exp.isUninitialized()) {
    *simplifiedExpression = Undefined::Builder();
    *approximateExpression = Undefined::Builder();
    return;
  }
  exp.simplifyAndApproximate(simplifiedExpression, approximateExpression, context, complexFormat, angleUnit, unitFormat, symbolicComputation, unitConversion);
  /* simplify might have been interrupted, in which case the resulting
   * expression is uninitialized, so we need to check that. */
  if (simplifiedExpression->isUninitialized()) {
    *simplifiedExpression = Parse(text, context);
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximate<double>(context, complexFormat, angleUnit);
    }
  }
}

Expression Expression::simplify(ExpressionNode::ReductionContext reductionContext) {
  sSimplificationHasBeenInterrupted = false;
  Expression e = reduce(reductionContext);
  if (!sSimplificationHasBeenInterrupted) {
    e = e.deepBeautify(reductionContext);
  }
  return sSimplificationHasBeenInterrupted ? Expression() : e;
}

void makePositive(Expression * e, bool * isNegative) {
  if (e->type() == ExpressionNode::Type::Opposite) {
    *isNegative = true;
    *e = e->childAtIndex(0);
  }
}

void Expression::beautifyAndApproximateScalar(Expression * simplifiedExpression, Expression * approximateExpression, ExpressionNode::ReductionContext userReductionContext, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  bool hasUnits = hasUnit();
  /* Case 1: the reduced expression is ComplexCartesian or pure real, we can
   * take into account the complex format to display a+i*b or r*e^(i*th) */
  if ((type() == ExpressionNode::Type::ComplexCartesian || isReal(context)) && !hasUnits) {
    ComplexCartesian ecomplex = type() == ExpressionNode::Type::ComplexCartesian ? convert<ComplexCartesian>() : ComplexCartesian::Builder(*this, Rational::Builder(0));
    if (approximateExpression) {
      /* Step 1: Approximation
       * We compute the approximate expression from the Cartesian form to avoid
       * imprecision. For example, if the result is the ComplexCartesian(a,b),
       * the final expression is going to be sqrt(a^2+b^2)*exp(i*atan(b/a)...
       * in Polar ComplexFormat. If we approximate this expression instead of
       * ComplexCartesian(a,b), we are going to loose precision on the resulting
       * complex.*/
      // Clone the ComplexCartesian to use it to compute the approximation
      ComplexCartesian ecomplexClone = ecomplex.clone().convert<ComplexCartesian>();
      // To minimize the error on the approximation, we reduce the number of nodes in the expression by beautifying
      ecomplexClone.real().deepBeautify(userReductionContext);
      ecomplexClone.imag().deepBeautify(userReductionContext);
      *approximateExpression = ecomplexClone.approximate<double>(context, complexFormat, angleUnit);
    }
    // Step 2: create the simplified expression with the required complex format
    Expression ra = complexFormat == Preferences::ComplexFormat::Polar ?
      ecomplex.clone().convert<ComplexCartesian>().norm(userReductionContext).shallowReduce(userReductionContext) :
      ecomplex.real();
    Expression tb = complexFormat == Preferences::ComplexFormat::Polar ?
      ecomplex.argument(userReductionContext).shallowReduce(userReductionContext) :
      ecomplex.imag();
    ra = ra.deepBeautify(userReductionContext);
    tb = tb.deepBeautify(userReductionContext);
    bool raIsNegative = false;
    bool tbIsNegative = false;
    makePositive(&ra, &raIsNegative);
    makePositive(&tb, &tbIsNegative);
    *simplifiedExpression = CreateComplexExpression(ra, tb, complexFormat, ra.isUndefined() || tb.isUndefined(), IsZero(ra), IsOne(ra), IsZero(tb), IsOne(tb), raIsNegative, tbIsNegative);
  } else {
    /* Case 2: The reduced expression has a complex component that could not
     * be bubbled up. */
    // Step 1: beautifying
    *simplifiedExpression = deepBeautify(userReductionContext);
    // Step 2: approximation
    if (approximateExpression) {
      if (hasUnits) {
        /* Approximate and simplified expressions are set equal so that only
         * one of them will be output. Note that there is no need to clone
         * since the expressions will not be altered. */
        *approximateExpression = *simplifiedExpression;
        return;
      }

      *approximateExpression = simplifiedExpression->approximate<double>(context, complexFormat, angleUnit);
    }
  }
}

void Expression::simplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation, ExpressionNode::UnitConversion unitConversion) {
  assert(simplifiedExpression);
  sSimplificationHasBeenInterrupted = false;
  // Step 1: we reduce the expression
  ExpressionNode::ReductionContext userReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::User, symbolicComputation, unitConversion);
  Expression e = clone().reduce(userReductionContext);
  if (sSimplificationHasBeenInterrupted) {
    sSimplificationHasBeenInterrupted = false;
    ExpressionNode::ReductionContext systemReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation, symbolicComputation, unitConversion);
    e = reduce(systemReductionContext);
  }
  *simplifiedExpression = Expression();
  if (sSimplificationHasBeenInterrupted) {
    return;
  }
  // Step 2: we approximate and beautify the reduced expression
  /* Case 1: the reduced expression is a matrix: We scan the matrix children to
   * beautify them with the right complex format. */
  if (e.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix &>(e);
    *simplifiedExpression = Matrix::Builder();
    if (approximateExpression) {
      *approximateExpression = Matrix::Builder();
    }
    for (int i = 0; i < e.numberOfChildren(); i++) {
      Expression simplifiedChild;
      Expression approximateChild = approximateExpression ? Expression() : nullptr;
      e.childAtIndex(i).beautifyAndApproximateScalar(&simplifiedChild, &approximateChild, userReductionContext, context, complexFormat, angleUnit);
      assert(!simplifiedChild.deepIsMatrix(context));
      static_cast<Matrix *>(simplifiedExpression)->addChildAtIndexInPlace(simplifiedChild, i, i);
      if (approximateExpression) {
        static_cast<Matrix *>(approximateExpression)->addChildAtIndexInPlace(approximateChild, i, i);
      }
    }
    static_cast<Matrix *>(simplifiedExpression)->setDimensions(m.numberOfRows(), m.numberOfColumns());
    if (approximateExpression) {
      static_cast<Matrix *>(approximateExpression)->setDimensions(m.numberOfRows(), m.numberOfColumns());
    }
  } else {
    /* Case 3: the reduced expression is scalar or too complex to respect the
     * complex format. */
    return e.beautifyAndApproximateScalar(simplifiedExpression, approximateExpression, userReductionContext, context, complexFormat, angleUnit);
  }
}

Expression Expression::ExpressionWithoutSymbols(Expression e, Context * context, bool replaceFunctionsOnly) {
  if (e.isUninitialized()) {
    return e;
  }
  /* Replace all the symbols iteratively. If we make too many replacements, the
   * symbols are likely to be defined circularly, in which case we return an
   * uninitialized expression.
   * We need a static "replacement count" to aggregate all calls to
   * ExpressionWithoutSymbols, as this method might be called from
   * hasReplaceableSymbols. */
  static int replacementCount = 0;
  bool unlock = false;
  if (!sSymbolReplacementsCountLock) {
    replacementCount = 0;
    sSymbolReplacementsCountLock = true;
    unlock = true;
  }
  bool didReplace;
  do {
    replacementCount++;
    if (replacementCount > k_maxSymbolReplacementsCount) {
      e = Expression();
      break;
    }
    didReplace = false;
    e = e.deepReplaceReplaceableSymbols(context, &didReplace, replaceFunctionsOnly, 0);
    if (e.isUninitialized()) { // the expression is circularly defined, escape
      replacementCount = k_maxSymbolReplacementsCount;
    }
  } while (didReplace);
  if (unlock) {
    sSymbolReplacementsCountLock = false;
  }
  return e;
}

Expression Expression::mapOnMatrixFirstChild(ExpressionNode::ReductionContext reductionContext) {
  /* For now, the matrix child on which the mapping must be done is always at
   * the index 0. */
  assert(childAtIndex(0).type() == ExpressionNode::Type::Matrix);
  Expression c = childAtIndex(0);
  Matrix matrix = Matrix::Builder();
  /* replace c with a ghost, because we will clone this and we do not want to
   * clone c, as it might be very big. */
  replaceChildInPlace(c, Ghost::Builder());
  for (int i = 0; i < c.numberOfChildren(); i++) {
    Expression f = clone();
    f.replaceChildAtIndexInPlace(0, c.childAtIndex(i));
    matrix.addChildAtIndexInPlace(f, i, i);
    f.shallowReduce(reductionContext);
  }
  matrix.setDimensions(static_cast<Matrix &>(c).numberOfRows(), static_cast<Matrix &>(c).numberOfColumns());
  replaceWithInPlace(matrix);
  return matrix.shallowReduce(reductionContext.context());
}

Expression Expression::radianToAngleUnit(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*180/Pi
    return Multiplication::Builder(*this, Rational::Builder(180), Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(-1)));
  }
  else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*200/Pi
    return Multiplication::Builder(*this, Rational::Builder(200), Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(-1)));
  }
  return *this;
}

Expression Expression::angleUnitToRadian(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*Pi/180
    return Multiplication::Builder(*this, Rational::Builder(1, 180), Constant::Builder(UCodePointGreekSmallLetterPi));
  }
  else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*Pi/200
    return Multiplication::Builder(*this, Rational::Builder(1, 200), Constant::Builder(UCodePointGreekSmallLetterPi));
  }
  return *this;
}

Expression Expression::reduceAndRemoveUnit(ExpressionNode::ReductionContext reductionContext, Expression * Unit) {
  /* RemoveUnit has to be called on reduced expression. reduce method is called
   * instead of deepReduce to catch interrupted simplification. */
  return reduce(reductionContext).removeUnit(Unit);
}

Expression Expression::reduce(ExpressionNode::ReductionContext reductionContext) {
  sSimplificationHasBeenInterrupted = false;
  Expression result = deepReduce(reductionContext);
  if (sSimplificationHasBeenInterrupted) {
    return replaceWithUndefinedInPlace();
  }
  return result;
}

Expression Expression::deepReduce(ExpressionNode::ReductionContext reductionContext) {
  deepReduceChildren(reductionContext);
  if (sSimplificationHasBeenInterrupted) {
    return *this;
  }
  return shallowReduce(reductionContext);
}

Expression Expression::deepBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression e = shallowBeautify(&reductionContext);
  e.deepBeautifyChildren(reductionContext);
  return e;
}

Expression Expression::setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive || s == ExpressionNode::Sign::Negative);
  return node()->setSign(s, reductionContext);
}

/* Evaluation */

template<typename U>
Expression Expression::approximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const {
  return isUninitialized() ? Undefined::Builder() : approximateToEvaluation<U>(context, complexFormat, angleUnit, withinReduce).complexToExpression(complexFormat);
}

template<typename U>
U Expression::approximateToScalar(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const {
  return approximateToEvaluation<U>(context, complexFormat, angleUnit, withinReduce).toScalar();
}

template<typename U>
U Expression::ApproximateToScalar(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) {
  Expression exp = ParseAndSimplify(text, context, complexFormat, angleUnit, unitFormat, symbolicComputation);
  assert(!exp.isUninitialized());
  return exp.approximateToScalar<U>(context, complexFormat, angleUnit);
}

template<typename U>
U Expression::approximateWithValueForSymbol(const char * symbol, U x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  VariableContext variableContext = VariableContext(symbol, context);
  variableContext.setApproximationForVariable<U>(x);
  return approximateToScalar<U>(&variableContext, complexFormat, angleUnit);
}

template<typename U>
U Expression::Epsilon() {
  static U epsilon = sizeof(U) == sizeof(double) ? 1E-15 : 1E-7f;
  return epsilon;
}

/* Builder */

bool Expression::IsZero(const Expression e) {
  return e.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(e).isZero();
}
bool Expression::IsOne(const Expression e) {
  return e.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(e).isOne();
}
bool Expression::IsMinusOne(const Expression e) {
  return e.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(e).isMinusOne();
}

Expression Expression::CreateComplexExpression(Expression ra, Expression tb, Preferences::ComplexFormat complexFormat, bool undefined, bool isZeroRa, bool isOneRa, bool isZeroTb, bool isOneTb, bool isNegativeRa, bool isNegativeTb) {
  if (undefined) {
    return Undefined::Builder();
  }
  switch (complexFormat) {
    case Preferences::ComplexFormat::Real:
    case Preferences::ComplexFormat::Cartesian:
    {
      Expression real;
      Expression imag;
      if (!isZeroRa || isZeroTb) {
        if (isNegativeRa) {
          real = Opposite::Builder(ra);
          real.shallowAddMissingParenthesis();
        } else {
          real = ra;
        }
      }
      if (!isZeroTb) {
        if (isOneTb) {
          imag = Constant::Builder(UCodePointMathematicalBoldSmallI);
        } else {
          imag = Multiplication::Builder(tb, Constant::Builder(UCodePointMathematicalBoldSmallI));
          imag.shallowAddMissingParenthesis();
        }
      }
      Expression result;
      if (imag.isUninitialized()) {
        return real;
      } else if (real.isUninitialized()) {
        if (isNegativeTb) {
          result = Opposite::Builder(imag);
        } else {
          return imag;
        }
      } else if (isNegativeTb) {
        result = Subtraction::Builder(real, imag);
      } else {
        result = Addition::Builder(real, imag);
      }
      result.shallowAddMissingParenthesis();
      return result;
    }
    default:
    {
      assert(complexFormat == Preferences::ComplexFormat::Polar);
      Expression norm;
      Expression exp;
      if (!isOneRa || isZeroTb) {
        /* Norm cannot be negative but can be preceded by a negative sign (for
         * instance "-log(0.3)") which would lead to isNegativeRa = True. */
        if (isNegativeRa) {
          norm = Opposite::Builder(ra);
        } else {
          norm = ra;
        }
      }
      if (!isZeroRa && !isZeroTb) {
        Expression arg;
        if (isOneTb) {
          arg = Constant::Builder(UCodePointMathematicalBoldSmallI);
        } else {
          arg = Multiplication::Builder(tb, Constant::Builder(UCodePointMathematicalBoldSmallI));
        }
        if (isNegativeTb) {
          arg = Opposite::Builder(arg);
        }
        arg.shallowAddMissingParenthesis();
        exp = Power::Builder(Constant::Builder(UCodePointScriptSmallE), arg);
        exp.shallowAddMissingParenthesis();
      }
      if (exp.isUninitialized()) {
        return norm;
      } else if (norm.isUninitialized()) {
        return exp;
      } else {
        Expression result = Multiplication::Builder(norm, exp);
        result.shallowAddMissingParenthesis();
        return result;
      }
    }
  }
}

/* Expression roots/extrema solver*/

Coordinate2D<double> Expression::nextMinimum(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return nextMinimumOfExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit);
}

Coordinate2D<double> Expression::nextMaximum(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Coordinate2D<double> minimumOfOpposite = nextMinimumOfExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        return -expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit);
  return Coordinate2D<double>(minimumOfOpposite.x1(), -minimumOfOpposite.x2());
}

double Expression::nextRoot(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* The algorithms used to numerically find roots require either the function
   * to change sign around the root or for the root to be an extremum. Neither
   * is true for the null function, which we handle here. */
  if (nullStatus(context) == ExpressionNode::NullStatus::Null) {
    return start + step;
  }
  return nextIntersectionWithExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit, nullptr);
}

Coordinate2D<double> Expression::nextIntersection(const char * symbol, double start, double step, double max, Poincare::Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  double resultAbscissa = nextIntersectionWithExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        const Expression * expression1 = reinterpret_cast<const Expression *>(context3);
        return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit)-expression1->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit, expression);
  Coordinate2D<double> result(resultAbscissa, approximateWithValueForSymbol(symbol, resultAbscissa, context, complexFormat, angleUnit));
  if (std::fabs(result.x2()) < std::fabs(step)*k_solverPrecision) {
    result.setX2(0.0);
  }
  return result;
}

Coordinate2D<double> Expression::nextMinimumOfExpression(const char * symbol, double start, double step, double max, Solver::ValueAtAbscissa evaluate, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression, bool lookForRootMinimum) const {
  Coordinate2D<double> result;
  if (start == max || step == 0.0) {
    return result;
  }
  double bracket[3];
  double x = start;
  bool endCondition = false;
  do {
    bracketMinimum(symbol, x, step, max, bracket, evaluate, context, complexFormat, angleUnit, expression);
    result = Solver::BrentMinimum(bracket[0], bracket[2], evaluate, context, complexFormat, angleUnit, this, symbol, &expression);
    x = bracket[1];
    // Because of float approximation, exact zero is never reached
    if (std::fabs(result.x1()) < std::fabs(step)*k_solverPrecision) {
      result.setX1(0);
      result.setX2(evaluate(0, context, complexFormat, angleUnit, this, symbol, &expression));
    }
    /* Ignore extremum whose value is undefined or too big because they are
     * really unlikely to be local extremum. */
    if (std::isnan(result.x2()) || std::fabs(result.x2()) > k_maxFloat) {
      result.setX1(NAN);
    }
    // Idem, exact 0 never reached
    if (std::fabs(result.x2()) < std::fabs(step)*k_solverPrecision) {
      result.setX2(0);
    }
    endCondition = std::isnan(result.x1()) && (step > 0.0 ? x <= max : x >= max);
    if (lookForRootMinimum) {
      endCondition |= std::fabs(result.x2()) > 0 && (step > 0.0 ? x <= max : x >= max);
    }
  } while (endCondition);
  if (lookForRootMinimum && std::fabs(result.x2()) > 0) {
    result.setX1(NAN);
  }
  return result;
}

void Expression::bracketMinimum(const char * symbol, double start, double step, double max, double result[3], Solver::ValueAtAbscissa evaluate, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  Coordinate2D<double> p[3] = {
    Coordinate2D<double>(start, evaluate(start, context, complexFormat, angleUnit, this, symbol, &expression)),
    Coordinate2D<double>(start+step, evaluate(start+step, context, complexFormat, angleUnit, this, symbol, &expression)),
    Coordinate2D<double>()
  };
  double x = start+2.0*step;
  while (step > 0.0 ? x <= max : x >= max) {
    p[2].setX1(x);
    p[2].setX2(evaluate(x, context, complexFormat, angleUnit, this, symbol, &expression));
    if ((p[0].x2() > p[1].x2() || std::isnan(p[0].x2()))
        && (p[2].x2() > p[1].x2() || std::isnan(p[2].x2()))
        && (!std::isnan(p[0].x2()) || !std::isnan(p[2].x2())))
    {
      result[0] = p[0].x1();
      result[1] = p[1].x1();
      result[2] = p[2].x1();
      return;
    }
    if (p[0].x2() > p[1].x2() && p[1].x2() == p[2].x2()) {
    } else {
      p[0] = p[1];
      p[1] = p[2];
    }
    x += step;
  }
  result[0] = NAN;
  result[1] = NAN;
  result[2] = NAN;
}

double Expression::nextIntersectionWithExpression(const char * symbol, double start, double step, double max, Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  if (start == max || step == 0.0) {
    return NAN;
  }
  double bracket[2];
  double result = NAN;
  static double precisionByGradUnit = 1E6;
  double x = start+step;
  do {
    bracketRoot(symbol, x, step, max, bracket, evaluation, context, complexFormat, angleUnit, expression);
    result = Solver::BrentRoot(bracket[0], bracket[1], std::fabs(step/precisionByGradUnit), evaluation, context, complexFormat, angleUnit, this, symbol, &expression);
    x = bracket[1];
  } while (std::isnan(result) && (step > 0.0 ? x <= max : x >= max));

  double extremumMax = std::isnan(result) ? max : result;
  Coordinate2D<double> resultExtremum[2] = {
    nextMinimumOfExpression(symbol, start, step, extremumMax,
        [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
          const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
          const char * symbol = reinterpret_cast<const char *>(context2);
          const Expression * expression1 = reinterpret_cast<const Expression *>(context3);
          return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit) - (expression1->isUninitialized() ? 0.0 : expression1->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit));
        }, context, complexFormat, angleUnit, expression, true),
    nextMinimumOfExpression(symbol, start, step, extremumMax,
        [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
          const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
          const char * symbol = reinterpret_cast<const char *>(context2);
          const Expression * expression1 = reinterpret_cast<const Expression *>(context3);
          return (expression1->isUninitialized() ? 0.0 : expression1->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit)) - expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
        }, context, complexFormat, angleUnit, expression, true)};
  for (int i = 0; i < 2; i++) {
    if (!std::isnan(resultExtremum[i].x1()) && (std::isnan(result) || std::fabs(result - start) > std::fabs(resultExtremum[i].x1() - start))) {
      result = resultExtremum[i].x1();
    }
  }
  if (std::fabs(result) < std::fabs(step)*k_solverPrecision) {
    result = 0;
  }
  return result;
}

void Expression::bracketRoot(const char * symbol, double start, double step, double max, double result[2], Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  double b = start;
  double c = start+step;
  double fb = evaluation(b, context, complexFormat, angleUnit, this, symbol, &expression);
  double fa = fb;
  double fc = evaluation(c, context, complexFormat, angleUnit, this, symbol, &expression);
  while (step > 0.0 ? c <= max : c >= max) {
    if (fb == 0. && ((fa < 0. && fc > 0.) || (fa > 0. && fc < 0.))) {
      /* If fb is null, we still check that the function changes sign on ]a,c[,
       * and that fa and fc are not null. Otherwise, it's more likely those
       * zeroes are caused by approximation errors. */
      result[0] = b;
      result[1] = c;
      return;
    } else if (fc != 0. && ((fb < 0.) != (fc < 0.))) {
      /* The function changes sign.
       * The case fc = 0 is handled in the next pass with fb = 0. */
      result[0] = b;
      result[1] = c;
      return;
    }
    fa = fb;
    b = c;
    fb = fc;
    c = c+step;
    fc = evaluation(c, context, complexFormat, angleUnit, this, symbol, &expression);
  }
  result[0] = NAN;
  result[1] = NAN;
}

template float Expression::Epsilon<float>();
template double Expression::Epsilon<double>();

template Expression Expression::approximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;
template Expression Expression::approximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;

template float Expression::approximateToScalar(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;
template double Expression::approximateToScalar(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;

template float Expression::ApproximateToScalar<float>(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation);
template double Expression::ApproximateToScalar<double>(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation);

template Evaluation<float> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;
template Evaluation<double> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;

template float Expression::approximateWithValueForSymbol(const char * symbol, float x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template double Expression::approximateWithValueForSymbol(const char * symbol, double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
