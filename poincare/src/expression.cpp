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

#include "parsing/parser.h"

namespace Poincare {

bool Expression::sSymbolReplacementsCountLock = false;
static bool sApproximationEncounteredComplex = false;

/* Constructor & Destructor */

Expression Expression::clone() const { TreeHandle c = TreeHandle::clone(); return static_cast<Expression&>(c); }

Expression Expression::Parse(char const * string, bool addParentheses) {
  if (string[0] == 0) {
    return Expression();
  }
  Parser p(string);
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

bool Expression::isRationalZero() const {
  return type() == ExpressionNode::Type::Rational && convert<const Rational>().isZero();
}

bool Expression::isRationalOne() const {
  return type() == ExpressionNode::Type::Rational && convert<const Rational>().isOne();
}

bool Expression::recursivelyMatches(ExpressionTest test, Context * context, bool replaceSymbols) const {
  if (test(*this, context)) {
    return true;
  }
  ExpressionNode::Type t = type();
  if (replaceSymbols && (t == ExpressionNode::Type::Symbol || t == ExpressionNode::Type::Function)) {
    return SymbolAbstract::matches(convert<const SymbolAbstract>(), test, context);
  }
  for (int i = 0; i < this->numberOfChildren(); i++) {
    if (childAtIndex(i).recursivelyMatches(test, context, replaceSymbols)) {
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
  if (isOfType(types1, 16)) {
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
  if (isOfType(types2, 4)) {
    assert(numberOfChildren() > 0);
    return childAtIndex(0).deepIsMatrix(context);
  }
  // By default, an expression is a matrix of any of its children is one (eg, Cosine, Decimal...)
  for (int i = 0; i < numberOfChildren(); i++) {
    if (childAtIndex(i).deepIsMatrix(context)) {
      return true;
    }
  }
  return false;
}

bool Expression::IsApproximate(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Decimal || e.type() == ExpressionNode::Type::Float;
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
    || e.type() == ExpressionNode::Type::MatrixTranspose;
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
  for (int i = 0; i < e.numberOfChildren(); i++) {
    if (containsVariables(e.childAtIndex(i), variables, maxVariableSize)) {
      return true;
    }
  }
  return false;
}

bool Expression::getLinearCoefficients(char * variables, int maxVariableSize, Expression coefficients[], Expression constant[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  assert(!recursivelyMatches(IsMatrix, context, true));
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
    int degree = equation.getPolynomialReducedCoefficients(&variables[index*maxVariableSize], polynomialCoefficients, context, complexFormat, angleUnit);
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
        assert(!recursivelyMatches([](const Expression e, Context * context) { return e.isUndefined(); }, context, true));
        return false;
    }
    /* The equation is can be written: a_1*x+a_0 with a_1 and a_0 x-independent.
     * The equation supposed to be linear in all variables, so we can look for
     * the coefficients linked to the other variables in a_0. */
    equation = polynomialCoefficients[0];
    index++;
  }
  constant[0] = Opposite::Builder(equation.clone()).reduce(context, complexFormat, angleUnit);
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

// Private

void Expression::shallowAddMissingParenthesis() {
  if (isUninitialized()) {
    return;
  }
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression child = childAtIndex(0);
    if (node()->childNeedsUserParentheses(child)) {
      replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}

Expression Expression::addMissingParentheses() {
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression child = childAtIndex(i).addMissingParentheses();
    if (node()->childNeedsUserParentheses(child)) {
      child = Parenthesis::Builder(child);
    }
    replaceChildAtIndexInPlace(i, child);
  }
  return *this;
}

void Expression::defaultDeepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  for (int i = 0; i < numberOfChildren(); i++) {
    childAtIndex(i).deepReduce(reductionContext);
  }
}

Expression Expression::defaultShallowReduce() {
  Expression result;
  for (int i = 0; i < numberOfChildren(); i++) {
    // The reduction is shortcutted if one child is unreal or undefined:
    // - the result is unreal is at least one child is unreal
    // - the result is undefined is at least one child is undefined but no child is unreal
    if (childAtIndex(i).type() == ExpressionNode::Type::Unreal) {
      result = Unreal::Builder();
      break;
    } else if (childAtIndex(i).type() == ExpressionNode::Type::Undefined) {
      result = Undefined::Builder();
    }
  }
  if (!result.isUninitialized()) {
    replaceWithInPlace(result);
    return result;
  }
  return *this;
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
  assert(numberOfChildren() == other.numberOfChildren());
  for (int i = 0; i < numberOfChildren(); i++) {
    replaceChildAtIndexInPlace(i, other.childAtIndex(i));
  }
}

bool Expression::hasReplaceableSymbols(Context * context) const {
  return recursivelyMatches([](const Expression e, Context * context) {
      return (e.type() == ExpressionNode::Type::Symbol
          && !static_cast<const Symbol &>(e).isSystemSymbol()
          && !context->expressionForSymbol(static_cast<const Symbol &>(e), false).isUninitialized())
      || (e.type() == ExpressionNode::Type::Function
           && !context->expressionForSymbol(static_cast<const Function &>(e), false).isUninitialized());
      }, context, false);
}

Expression Expression::defaultReplaceReplaceableSymbols(Context * context) {
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    childAtIndex(i).shallowReplaceReplaceableSymbols(context);
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
    return m;
  }
  return Expression();
}

template<typename U>
Evaluation<U> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  sApproximationEncounteredComplex = false;
  // Reset interrupting flag because some evaluation methods use it
  sSimplificationHasBeenInterrupted = false;
  Evaluation<U> e = node()->approximate(U(), context, complexFormat, angleUnit);
  if (complexFormat == Preferences::ComplexFormat::Real && sApproximationEncounteredComplex) {
    e = Complex<U>::Undefined();
  }
  return e;
}

Expression Expression::defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression) {
  for (int i = 0; i < numberOfChildren(); i++) {
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

int Expression::getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  // Reset interrupting flag because we use deepReduce
  int degree = getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].reduce(context, complexFormat, angleUnit);
  }
  return degree;
}

Expression Expression::replaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol) {
  return node()->replaceUnknown(symbol, unknownSymbol);
}

Expression Expression::defaultReplaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol) {
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    childAtIndex(i).replaceUnknown(symbol, unknownSymbol);
  }
  return *this;
}

/* Complex */

bool Expression::EncounteredComplex() {
  return sApproximationEncounteredComplex;
}

void Expression::SetEncounteredComplex(bool encounterComplex) {
  sApproximationEncounteredComplex = encounterComplex;
}

Preferences::ComplexFormat Expression::UpdatedComplexFormatWithTextInput(Preferences::ComplexFormat complexFormat, const char * textInput) {
  if (complexFormat == Preferences::ComplexFormat::Real && *(UTF8Helper::CodePointSearch(textInput, UCodePointMathematicalBoldSmallI)) != 0) {
    return Preferences::ComplexFormat::Cartesian;
  }
  return complexFormat;
}

Preferences::ComplexFormat Expression::UpdatedComplexFormatWithExpressionInput(Preferences::ComplexFormat complexFormat, const Expression & exp, Context * context) {
  if (complexFormat == Preferences::ComplexFormat::Real && exp.recursivelyMatches([](const Expression e, Context * context) { return e.type() == ExpressionNode::Type::Constant && static_cast<const Constant &>(e).isIComplex(); }, context, true)) {
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
  if (isOfType(types1, 5)) {
    return childAtIndex(0).isReal(context);
  }

  // These expressions are always real
  ExpressionNode::Type types2[] = {ExpressionNode::Type::BinomialCoefficient, ExpressionNode::Type::Derivative, ExpressionNode::Type::DivisionQuotient, ExpressionNode::Type::DivisionRemainder, ExpressionNode::Type::GreatCommonDivisor, ExpressionNode::Type::Integral, ExpressionNode::Type::LeastCommonMultiple, ExpressionNode::Type::PermuteCoefficient, ExpressionNode::Type::Randint, ExpressionNode::Type::Random, ExpressionNode::Type::Round, ExpressionNode::Type::SignFunction};
  if ((isNumber() && !isUndefined()) || isOfType(types2, 12)) {
    return true;
  }

  // These expressions are real when they are scalar
  ExpressionNode::Type types3[] = {ExpressionNode::Type::AbsoluteValue, ExpressionNode::Type::Ceiling, ExpressionNode::Type::ComplexArgument, ExpressionNode::Type::Factorial, ExpressionNode::Type::Floor, ExpressionNode::Type::FracPart, ExpressionNode::Type::ImaginaryPart, ExpressionNode::Type::RealPart};
  if (isOfType(types3, 8)) {
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
   * order on expresssions. */
  return ExpressionNode::SimplificationOrder(node(), e.node(), true, true) == 0;
}

bool Expression::isEqualToItsApproximationLayout(Expression approximation, char * buffer, int bufferSize, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) {
  approximation.serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  /* Warning: we cannot use directly the the approximate expression but we have
   * to re-serialize it because the number of stored significative
   * numbers and the number of displayed significative numbers might not be
   * identical. (For example, 0.000025 might be displayed "0.00003" and stored
   * as Decimal(0.000025) and isEqualToItsApproximationLayout should return
   * false) */
  Expression approximateOutput = Expression::ParseAndSimplify(buffer, context, complexFormat, angleUnit, true);
  bool equal = isIdenticalTo(approximateOutput);
  return equal;
}

/* Layout Helper */

Layout Expression::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return isUninitialized() ? Layout() : node()->createLayout(floatDisplayMode, numberOfSignificantDigits);
}

int Expression::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return isUninitialized() ? 0 : node()->serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits); }

/* Simplification */

Expression Expression::ParseAndSimplify(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicSimplification) {
  Expression exp = Parse(text, false);
  if (exp.isUninitialized()) {
    return Undefined::Builder();
  }
  exp = exp.simplify(context, complexFormat, angleUnit, symbolicSimplification);
  /* simplify might have been interrupted, in which case the resulting
   * expression is uninitialized, so we need to check that. */
  if (exp.isUninitialized()) {
    return Parse(text);
  }
  return exp;
}

void Expression::ParseAndSimplifyAndApproximate(const char * text, Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicComputation) {
  assert(simplifiedExpression);
  Expression exp = Parse(text, false);
  if (exp.isUninitialized()) {
    *simplifiedExpression = Undefined::Builder();
    *approximateExpression = Undefined::Builder();
    return;
  }
  exp.simplifyAndApproximate(simplifiedExpression, approximateExpression, context, complexFormat, angleUnit, symbolicComputation);
  /* simplify might have been interrupted, in which case the resulting
   * expression is uninitialized, so we need to check that. */
  if (simplifiedExpression->isUninitialized()) {
    *simplifiedExpression = Parse(text);
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximate<double>(context, complexFormat, angleUnit);
    }
  }
}

Expression Expression::simplify(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicComputation) {
  sSimplificationHasBeenInterrupted = false;
  ExpressionNode::ReductionContext c = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::System, symbolicComputation);
  Expression e = deepReduce(c);
  if (!sSimplificationHasBeenInterrupted) {
    e = e.deepBeautify(c);
  }
  return sSimplificationHasBeenInterrupted ? Expression() : e;
}

void makePositive(Expression * e, bool * isNegative) {
  if (e->type() == ExpressionNode::Type::Opposite) {
    *isNegative = true;
    *e = e->childAtIndex(0);
  }
}

void Expression::simplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicComputation) {
  assert(simplifiedExpression);
  sSimplificationHasBeenInterrupted = false;
  // Step 1: we reduce the expression
  ExpressionNode::ReductionContext userReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User, symbolicComputation);
  Expression e = clone().deepReduce(userReductionContext);
  if (sSimplificationHasBeenInterrupted) {
    sSimplificationHasBeenInterrupted = false;
    ExpressionNode::ReductionContext systemReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::System, symbolicComputation);
    e = deepReduce(systemReductionContext);
  }
  *simplifiedExpression = Expression();
  if (sSimplificationHasBeenInterrupted) {
    return;
  }
  /* Case 1: the reduced expression is ComplexCartesian or pure real, we can
   * take into account the complex format to display a+i*b or r*e^(i*th) */
  if (e.type() == ExpressionNode::Type::ComplexCartesian || e.isReal(context)) {
    ComplexCartesian ecomplex = e.type() == ExpressionNode::Type::ComplexCartesian ? static_cast<ComplexCartesian &>(e) : ComplexCartesian::Builder(e, Rational::Builder(0));
    if (approximateExpression) {
      /* Step 2: Approximation
       * We compute the approximate expression from the Cartesian form to avoid
       * unprecision. For example, if the result is the ComplexCartesian(a,b),
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
    // Step 3: create the simplied expression with the required complex format
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
    *simplifiedExpression = e.deepBeautify(userReductionContext);
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximate<double>(context, complexFormat, angleUnit);
    }
  }
}

Expression Expression::ExpressionWithoutSymbols(Expression e, Context * context) {
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
  while (e.hasReplaceableSymbols(context)) {
    replacementCount++;
    if (replacementCount > k_maxSymbolReplacementsCount) {
      e = Expression();
      break;
    }
    e = e.shallowReplaceReplaceableSymbols(context);
  }
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
  return matrix.shallowReduce(reductionContext);
}

Expression Expression::radianToDegree() {
  // e*180/Pi
  return Multiplication::Builder(*this, Rational::Builder(180), Power::Builder(Constant::Builder(UCodePointGreekSmallLetterPi), Rational::Builder(-1)));
}

Expression Expression::degreeToRadian() {
  // e*Pi/180
  return Multiplication::Builder(*this, Rational::Builder(1, 180), Constant::Builder(UCodePointGreekSmallLetterPi));
}

Expression Expression::reduce(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  sSimplificationHasBeenInterrupted = false;
  return deepReduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::System, true));
}

Expression Expression::deepReduce(ExpressionNode::ReductionContext reductionContext) {
  deepReduceChildren(reductionContext);
  if (sSimplificationHasBeenInterrupted) {
    return *this;
  }
  return shallowReduce(reductionContext);
}

Expression Expression::deepBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression e = shallowBeautify(reductionContext);
  int nbChildren = e.numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    Expression child = e.childAtIndex(i);
    child = child.deepBeautify(reductionContext);
    // We add missing Parentheses after beautifying the parent and child
    if (e.node()->childNeedsUserParentheses(child)) {
      e.replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
  return e;
}

Expression Expression::setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive || s == ExpressionNode::Sign::Negative);
  return node()->setSign(s, reductionContext);
}

/* Evaluation */

template<typename U>
Expression Expression::approximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return isUninitialized() ? Undefined::Builder() : approximateToEvaluation<U>(context, complexFormat, angleUnit).complexToExpression(complexFormat);
}


template<typename U>
U Expression::approximateToScalar(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return approximateToEvaluation<U>(context, complexFormat, angleUnit).toScalar();
}

template<typename U>
U Expression::ApproximateToScalar(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicSimplification) {
  Expression exp = ParseAndSimplify(text, context, complexFormat, angleUnit, symbolicSimplification);
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
        assert(!isNegativeRa); // norm cannot be negative
        norm = ra;
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

Coordinate2D Expression::nextMinimum(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return nextMinimumOfExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit);
}

Coordinate2D Expression::nextMaximum(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Coordinate2D minimumOfOpposite = nextMinimumOfExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        return -expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit);
  return Coordinate2D(minimumOfOpposite.abscissa(), -minimumOfOpposite.value());
}

double Expression::nextRoot(const char * symbol, double start, double step, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return nextIntersectionWithExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit, nullptr);
}

Coordinate2D Expression::nextIntersection(const char * symbol, double start, double step, double max, Poincare::Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  double resultAbscissa = nextIntersectionWithExpression(symbol, start, step, max,
      [](double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const Expression * expression0 = reinterpret_cast<const Expression *>(context1);
        const char * symbol = reinterpret_cast<const char *>(context2);
        const Expression * expression1 = reinterpret_cast<const Expression *>(context3);
        return expression0->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit)-expression1->approximateWithValueForSymbol(symbol, x, context, complexFormat, angleUnit);
      }, context, complexFormat, angleUnit, expression);
  Coordinate2D result(resultAbscissa, approximateWithValueForSymbol(symbol, resultAbscissa, context, complexFormat, angleUnit));
  if (std::fabs(result.value()) < step*k_solverPrecision) {
    result.setValue(0.0);
  }
  return result;
}

Coordinate2D Expression::nextMinimumOfExpression(const char * symbol, double start, double step, double max, Solver::ValueAtAbscissa evaluate, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression, bool lookForRootMinimum) const {
  Coordinate2D result;
  if (start == max || step == 0.0) {
    return result;
  }
  double bracket[3];
  double x = start;
  bool endCondition = false;
  do {
    bracketMinimum(symbol, x, step, max, bracket, evaluate, context, complexFormat, angleUnit, expression);
    result = brentMinimum(symbol, bracket[0], bracket[2], evaluate, context, complexFormat, angleUnit, expression);
    x = bracket[1];
    // Because of float approximation, exact zero is never reached
    if (std::fabs(result.abscissa()) < std::fabs(step)*k_solverPrecision) {
      result.setAbscissa(0);
      result.setValue(evaluate(0, context, complexFormat, angleUnit, this, symbol, &expression));
    }
    /* Ignore extremum whose value is undefined or too big because they are
     * really unlikely to be local extremum. */
    if (std::isnan(result.value()) || std::fabs(result.value()) > k_maxFloat) {
      result.setAbscissa(NAN);
    }
    // Idem, exact 0 never reached
    if (std::fabs(result.value()) < std::fabs(step)*k_solverPrecision) {
      result.setValue(0);
    }
    endCondition = std::isnan(result.abscissa()) && (step > 0.0 ? x <= max : x >= max);
    if (lookForRootMinimum) {
      endCondition |= std::fabs(result.value()) > 0 && (step > 0.0 ? x <= max : x >= max);
    }
  } while (endCondition);
  if (lookForRootMinimum && std::fabs(result.value()) > 0) {
    result.setAbscissa(NAN);
  }
  return result;
}

void Expression::bracketMinimum(const char * symbol, double start, double step, double max, double result[3], Solver::ValueAtAbscissa evaluate, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  Coordinate2D p[3] = {
    Coordinate2D(start, evaluate(start, context, complexFormat, angleUnit, this, symbol, &expression)),
    Coordinate2D(start+step, evaluate(start+step, context, complexFormat, angleUnit, this, symbol, &expression)),
    Coordinate2D()
  };
  double x = start+2.0*step;
  while (step > 0.0 ? x <= max : x >= max) {
    p[2].setAbscissa(x);
    p[2].setValue(evaluate(x, context, complexFormat, angleUnit, this, symbol, &expression));
    if ((p[0].value() > p[1].value() || std::isnan(p[0].value()))
        && (p[2].value() > p[1].value() || std::isnan(p[2].value()))
        && (!std::isnan(p[0].value()) || !std::isnan(p[2].value())))
    {
      result[0] = p[0].abscissa();
      result[1] = p[1].abscissa();
      result[2] = p[2].abscissa();
      return;
    }
    if (p[0].value() > p[1].value() && p[1].value() == p[2].value()) {
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

Coordinate2D Expression::brentMinimum(const char * symbol, double ax, double bx, Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  return Solver::BrentMinimum(
      ax,
      bx,
      evaluation,
      context,
      complexFormat,
      angleUnit,
      this,
      symbol,
      &expression);
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
    result = brentRoot(symbol, bracket[0], bracket[1], std::fabs(step/precisionByGradUnit), evaluation, context, complexFormat, angleUnit, expression);
    x = bracket[1];
  } while (std::isnan(result) && (step > 0.0 ? x <= max : x >= max));

  double extremumMax = std::isnan(result) ? max : result;
  Coordinate2D resultExtremum[2] = {
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
    if (!std::isnan(resultExtremum[i].abscissa()) && (std::isnan(result) || std::fabs(result - start) > std::fabs(resultExtremum[i].abscissa() - start))) {
      result = resultExtremum[i].abscissa();
    }
  }
  if (std::fabs(result) < std::fabs(step)*k_solverPrecision) {
    result = 0;
  }
  return result;
}

void Expression::bracketRoot(const char * symbol, double start, double step, double max, double result[2], Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  double a = start;
  double b = start+step;
  while (step > 0.0 ? b <= max : b >= max) {
    double fa = evaluation(a, context, complexFormat, angleUnit, this, symbol, &expression);
    double fb = evaluation(b, context, complexFormat, angleUnit, this, symbol, &expression);
    if (fa*fb <= 0) {
      result[0] = a;
      result[1] = b;
      return;
    }
    a = b;
    b = b+step;
  }
  result[0] = NAN;
  result[1] = NAN;
}

double Expression::brentRoot(const char * symbol, double ax, double bx, double precision, Solver::ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression) const {
  return Solver::BrentRoot(
      ax,
      bx,
      precision,
      evaluation,
      context,
      complexFormat,
      angleUnit,
      this,
      symbol,
      &expression);
}

template float Expression::Epsilon<float>();
template double Expression::Epsilon<double>();

template Expression Expression::approximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Expression Expression::approximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

template float Expression::approximateToScalar(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) const;
template double Expression::approximateToScalar(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) const;

template float Expression::ApproximateToScalar<float>(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicComputation);
template double Expression::ApproximateToScalar<double>(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool symbolicComputation);

template Evaluation<float> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) const;

template float Expression::approximateWithValueForSymbol(const char * symbol, float x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template double Expression::approximateWithValueForSymbol(const char * symbol, double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
