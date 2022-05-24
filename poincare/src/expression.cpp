#include <poincare/expression.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression_node.h>
#include <poincare/addition.h>
#include <poincare/based_integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/comparison_operator.h>
#include <poincare/constant.h>
#include <poincare/decimal.h>
#include <poincare/dependency.h>
#include <poincare/derivative.h>
#include <poincare/ghost.h>
#include <poincare/imaginary_part.h>
#include <poincare/list.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/real_part.h>
#include <poincare/store.h>
#include <poincare/string_layout.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/string_layout.h>
#include <poincare/variable_context.h>
#include <ion.h>
#include <ion/unicode/utf8_helper.h>
#include <cmath>
#include <float.h>
#include <utility>

#include "parsing/parser.h"

namespace Poincare {

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

/* Hierarchy */

Expression Expression::childAtIndex(int i) const {
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Expression &>(c);
}

/* Properties */

bool Expression::isRationalOne() const {
  return type() == ExpressionNode::Type::Rational && convert<const Rational>().isOne();
}

bool Expression::recursivelyMatches(ExpressionTernaryTest test, Context * context, ExpressionNode::SymbolicComputation replaceSymbols, void * auxiliary) const {
  RecursiveSearchResult testResult = test(*this, context, auxiliary);
  if (testResult == RecursiveSearchResult::Yes) {
    return true;
  } else if (testResult == RecursiveSearchResult::No) {
    return false;
  }
  assert(testResult == RecursiveSearchResult::Maybe);

  // Handle dependencies, symbols and functions
  ExpressionNode::Type t = type();
  if (t == ExpressionNode::Type::Dependency) {
    Expression e = *this;
    return static_cast<Dependency &>(e).dependencyRecursivelyMatches(test, context, replaceSymbols, auxiliary);
  }
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
      return SymbolAbstract::matches(convert<const SymbolAbstract>(), test, context, auxiliary);
  }

  const int childrenCount = this->numberOfChildren();
  // Run loop backwards to find lists and matrices quicker in NAry expressions
  for (int i = childrenCount - 1; i >= 0; i--) {
    if (childAtIndex(i).recursivelyMatches(test, context, replaceSymbols, auxiliary)) {
      return true;
    }
  }
  return false;
}

bool Expression::recursivelyMatches(ExpressionTest test, Context * context, ExpressionNode::SymbolicComputation replaceSymbols) const {
  ExpressionTernaryTest ternary = [](const Expression e, Context * context, void * auxiliary) {
    ExpressionTest * trueTest = static_cast<ExpressionTest *>(auxiliary);
    return (*trueTest)(e, context) ? RecursiveSearchResult::Yes : RecursiveSearchResult::Maybe;
  };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool Expression::hasExpression(ExpressionTypeTest test, const void * context) const {
  if (test(*this, context)) {
    return true;
  }
  if (isUninitialized()) {
    return false;
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
  return recursivelyMatches([](const Expression e, Context * context, void *) {
        if (IsMatrix(e, context)) { return RecursiveSearchResult::Yes; }
        // The children were sorted so any expression which is a matrix (deeply) would be at the end
        if (IsNAry(e, context) && e.numberOfChildren() > 0) {
          return e.childAtIndex(e.numberOfChildren() - 1).deepIsMatrix(context) ? RecursiveSearchResult::Yes : RecursiveSearchResult::No;
        }
         /* Dependency are matrices only if their first child is a matrix */
        if (e.type() == ExpressionNode::Type::Dependency) {
           return e.childAtIndex(0).deepIsMatrix(context) ? RecursiveSearchResult::Yes : RecursiveSearchResult::No;
        }
        // These types are matrices if one of their children is one
        if (e.isOfType({ExpressionNode::Type::Power, ExpressionNode::Type::Opposite, ExpressionNode::Type::Sum, ExpressionNode::Type::Product})) {
          return RecursiveSearchResult::Maybe;
        }
        // Any other type is not a matrix
        return RecursiveSearchResult::No;
      }, context);
 }

bool Expression::deepIsList(Context * context) const {
  return recursivelyMatches([](const Expression e, Context * context, void *) {
      switch (e.type()) {
      /* These expressions are always lists. */
      case ExpressionNode::Type::List:
      case ExpressionNode::Type::ListElement:
      case ExpressionNode::Type::ListSlice:
      case ExpressionNode::Type::ListSort:
      return RecursiveSearchResult::Yes;

      /* These expressions have a list as argument but are never lists, we
       * must stop the search. */
      case ExpressionNode::Type::Dimension:
      case ExpressionNode::Type::ListMaximum:
      case ExpressionNode::Type::ListMean:
      case ExpressionNode::Type::ListMedian:
      case ExpressionNode::Type::ListMinimum:
      case ExpressionNode::Type::ListProduct:
      case ExpressionNode::Type::ListSampleStandardDeviation:
      case ExpressionNode::Type::ListStandardDeviation:
      case ExpressionNode::Type::ListSum:
      case ExpressionNode::Type::ListVariance:
      return RecursiveSearchResult::No;

      /* Other expressions may be lists if their children are lists. */
      default:
      return RecursiveSearchResult::Maybe;
      }
  }, context);
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
    /* A Dimension is a matrix unless its child is a list. */
    || (e.type() == ExpressionNode::Type::Dimension && !e.childAtIndex(0).deepIsList(context))
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

bool Expression::IsPercent(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Percent;
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

bool Expression::getLinearCoefficients(char * variables, int maxVariableSize, Expression coefficients[], Expression * constant, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation) const {
  assert(!recursivelyMatches(IsMatrix, context, symbolicComputation));
  // variables is in fact of type char[k_maxNumberOfVariables][maxVariableSize]
  int numberOfVariables = 0;
  while (numberOfVariables < k_maxNumberOfVariables && variables[numberOfVariables*maxVariableSize] != 0) {
    int degree = polynomialDegree(context, &variables[numberOfVariables*maxVariableSize]);
    if (degree > 1 || degree < 0) {
      return false;
    }
    numberOfVariables++;
  }
  Expression equation = *this;
  Expression polynomialCoefficients[k_maxNumberOfPolynomialCoefficients];
  for (size_t index = 0; index < numberOfVariables; index++) {
    assert(variables[index*maxVariableSize] != 0);
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
  }
  *constant = Opposite::Builder(equation).cloneAndReduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation, symbolicComputation));
  /* The expression can be linear on all coefficients taken one by one but
   * non-linear (ex: xy = 2). We delete the results and return false if one of
   * the coefficients contains a variable. */
  bool isMultivariablePolynomial = containsVariables(*constant, variables, maxVariableSize);
  for (int i = 0; i < numberOfVariables; i++) {
    if (isMultivariablePolynomial) {
      break;
    }
    isMultivariablePolynomial = containsVariables(coefficients[i], variables, maxVariableSize);
  }
  return !isMultivariablePolynomial;
}

bool Expression::allChildrenAreUndefined() {
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (!childAtIndex(i).isUndefined()) {
      return false;
    }
  }
  return true;
}

bool Expression::isDefinedCosineOrSine(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (isOfType({ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine})) {
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

bool Expression::derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  return node()->derivate(reductionContext, symbol, symbolValue);
}

void Expression::derivateChildAtIndexInPlace(int index, ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue) {
  if (!childAtIndex(index).derivate(reductionContext, symbol, symbolValue)) {
    replaceChildAtIndexInPlace(index, Derivative::Builder(childAtIndex(index), symbol.clone().convert<Symbol>(), symbolValue.clone()));
  }
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

Expression Expression::shallowReduceUsingApproximation(ExpressionNode::ReductionContext reductionContext) {
  double approx = node()->approximate(double(), ExpressionNode::ApproximationContext(reductionContext, true)).toScalar();
  /* If approx is capped by the largest integer such as all smaller integers can
   * be exactly represented in IEEE754, approx is the exact result (no
   * precision were loss). */
  if (!std::isnan(approx) && std::fabs(approx) <= k_largestExactIEEE754Integer) {
    Expression result = Decimal::Builder(approx);
    replaceWithInPlace(result);
    result = result.shallowReduce(reductionContext);
    assert(result.type() == ExpressionNode::Type::Rational);
    return result;
  }
  return *this;
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

Expression Expression::defaultReplaceReplaceableSymbols(Context * context, bool * isCircular, int maxSymbolsToReplace, int parameteredAncestorsCount, ExpressionNode::SymbolicComputation symbolicComputation) {
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    childAtIndex(i).deepReplaceReplaceableSymbols(context, isCircular, maxSymbolsToReplace, parameteredAncestorsCount, symbolicComputation);
    if (*isCircular) {
      // the expression is circularly defined, escape
      return *this;
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
  int degree = getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].cloneAndReduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::SystemForApproximation, symbolicComputation));
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
  if (complexFormat == Preferences::ComplexFormat::Real && UTF8Helper::HasCodePoint(textInput, 'i')) {
    return Preferences::ComplexFormat::Cartesian;
  }
  return complexFormat;
}

Preferences::ComplexFormat Expression::UpdatedComplexFormatWithExpressionInput(Preferences::ComplexFormat complexFormat, const Expression & exp, Context * context) {
  if (complexFormat == Preferences::ComplexFormat::Real && exp.hasComplexI(context)) {
    return Preferences::ComplexFormat::Cartesian;
  }
  return complexFormat;
}

bool Expression::hasComplexI(Context * context) const {
  return recursivelyMatches(
      [](const Expression e, Context * context) {
        return e.type() == ExpressionNode::Type::ConstantMaths
               && static_cast<const Constant &>(e).isConstant("i");
      },
      context);
}

bool Expression::isReal(Context * context) const {
  /* We could do something virtual instead of implementing a disjunction on
   * types but many expressions have the same implementation so it is easier to
   * factorize it here. */

  // These expressions are real if their children are
  if (isOfType({ExpressionNode::Type::ArcTangent, ExpressionNode::Type::Conjugate, ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine, ExpressionNode::Type::Tangent})) {
    return childAtIndex(0).isReal(context);
  }

  // These expressions are always real
  if ((isNumber() && !isUndefined()) || isOfType({ExpressionNode::Type::BinomialCoefficient, ExpressionNode::Type::Derivative, ExpressionNode::Type::DivisionQuotient, ExpressionNode::Type::DivisionRemainder, ExpressionNode::Type::GreatCommonDivisor, ExpressionNode::Type::Integral, ExpressionNode::Type::LeastCommonMultiple, ExpressionNode::Type::PermuteCoefficient, ExpressionNode::Type::Randint, ExpressionNode::Type::Random, ExpressionNode::Type::Round, ExpressionNode::Type::SignFunction, ExpressionNode::Type::Unit})) {
    return true;
  }

  // These expressions are real when they are scalar
  if (isOfType({ExpressionNode::Type::AbsoluteValue, ExpressionNode::Type::Ceiling, ExpressionNode::Type::ComplexArgument, ExpressionNode::Type::Factorial, ExpressionNode::Type::Floor, ExpressionNode::Type::FracPart, ExpressionNode::Type::ImaginaryPart, ExpressionNode::Type::RealPart})) {
    return !deepIsMatrix(context);
  }

  // NAryExpresions are real if all children are real
  if (IsNAry(*this, context)) {
    return convert<NAryExpression>().allChildrenAreReal(context) == 1;
  }

  if (type() == ExpressionNode::Type::ConstantMaths) {
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
  return ExpressionNode::SimplificationOrder(node(), e.node(), true) == 0;
}

bool Expression::isIdenticalToWithoutParentheses(const Expression e) const {
  // Same as isIdenticalTo, but ignoring the parentheses.
  return ExpressionNode::SimplificationOrder(node(), e.node(), true, true) == 0;
}

bool Expression::ParsedExpressionsAreEqual(const char * e0, const char * e1, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat) {
  Expression exp0 = Expression::ParseAndSimplify(e0, context, complexFormat, angleUnit, unitFormat, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  Expression exp1 = Expression::ParseAndSimplify(e1, context, complexFormat, angleUnit, unitFormat, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  assert(!exp0.isUninitialized() && !exp1.isUninitialized());
  return exp0.isIdenticalTo(exp1);
}

/* Layout Helper */

static bool hasStringWithThousandSeparator(Layout l) {
  if (l.type() == LayoutNode::Type::StringLayout) {
    return static_cast<StringLayoutNode *>(l.node())->numberOfThousandsSeparators() > 0;
  }
  int n = l.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (hasStringWithThousandSeparator(l.childAtIndex(i))) {
      return true;
    }
  }
  return false;
}

static void stripMargin(Layout l) {
  l.node()->setMargin(false);
  int n = l.numberOfChildren();
  for (int i = 0; i < n; i++) {
    stripMargin(l.childAtIndex(i));
  }
}

Layout Expression::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, bool stripMarginStyle, bool nested) const {
  if (isUninitialized()) {
    return Layout();
  }
  Layout l = node()->createLayout(floatDisplayMode, numberOfSignificantDigits);
  assert(!l.isUninitialized());
  if (stripMarginStyle
   || !(nested || hasStringWithThousandSeparator(l))) {
    stripMargin(l);
  }
  return l;
}

int Expression::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return isUninitialized() ? 0 : node()->serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits); }

/* Simplification */

Expression Expression::ParseAndSimplify(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation, ExpressionNode::UnitConversion unitConversion) {
  Expression exp = Parse(text, context, false);
  if (exp.isUninitialized()) {
    return Undefined::Builder();
  }
  exp = exp.cloneAndSimplify(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::User, symbolicComputation, unitConversion));
  assert(!exp.isUninitialized());
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
  exp.cloneAndSimplifyAndApproximate(simplifiedExpression, approximateExpression, context, complexFormat, angleUnit, unitFormat, symbolicComputation, unitConversion);
  assert(!simplifiedExpression->isUninitialized() && (!approximateExpression || !approximateExpression->isUninitialized()));
}

Expression Expression::cloneAndSimplify(ExpressionNode::ReductionContext reductionContext) {
  bool reduceFailure = false;
  Expression e = cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
  if (reduceFailure) {
    // We can't beautify unreduced expression
    return e;
  }
  return e.deepBeautify(reductionContext);
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

void Expression::SimplifyAndApproximateChildren(Expression input, Expression * simplifiedOutput, Expression * approximateOutput, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionContext reductionContext) {
  assert(input.type() == ExpressionNode::Type::Matrix || input.type() == ExpressionNode::Type::List);
  List simplifiedChildren = List::Builder(), approximatedChildren = List::Builder();
  int n = input.numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression simplifiedChild, approximateChild;
    Expression * approximateChildAddress = approximateOutput ? &approximateChild : nullptr;
    Expression childI = input.childAtIndex(i);
    childI.beautifyAndApproximateScalar(&simplifiedChild, approximateChildAddress, reductionContext, context, complexFormat, angleUnit);
    simplifiedChildren.addChildAtIndexInPlace(simplifiedChild, i, i);
    if (approximateOutput) {
      assert(!approximateChild.isUninitialized());
      /* Clone the child in case it was set to the same node as simplified
       * child. This can happen when beautifying an unreduced matrix. */
      approximatedChildren.addChildAtIndexInPlace(approximateChild.clone(), i, i);
    }
  }

  if (input.type() == ExpressionNode::Type::List) {
    *simplifiedOutput = simplifiedChildren;
    if (approximateOutput) {
      *approximateOutput = approximatedChildren;
    }
    return;
  }

  assert(input.type() == ExpressionNode::Type::Matrix);
  Matrix simplifiedMatrix = Matrix::Builder(), approximateMatrix = Matrix::Builder();
  for (int i = 0; i < n; i++) {
    simplifiedMatrix.addChildAtIndexInPlace(simplifiedChildren.childAtIndex(i), i, i);
    if (approximateOutput) {
      approximateMatrix.addChildAtIndexInPlace(approximatedChildren.childAtIndex(i), i, i);
    }
  }
  Matrix m = static_cast<Matrix &>(input);
  simplifiedMatrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  *simplifiedOutput = simplifiedMatrix;
  if (approximateOutput) {
    approximateMatrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
    *approximateOutput = approximateMatrix;
  }
}

void Expression::cloneAndSimplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::SymbolicComputation symbolicComputation, ExpressionNode::UnitConversion unitConversion) const {
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  assert(!approximateExpression || approximateExpression->isUninitialized());

  // Step 1: we reduce the expression
  /* We tried first with the ReductionTarget::User. If the reduction failed
   * without any user interruption (too many nodes were generated), we try
   * again with ReductionTarget::SystemForApproximation. */
  ExpressionNode::ReductionContext userReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, ExpressionNode::ReductionTarget::User, symbolicComputation, unitConversion);
  ExpressionNode::ReductionContext reductionContext = userReductionContext;
  bool reduceFailure = false;
  Expression e = cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
  if (reduceFailure || (type() == ExpressionNode::Type::Store && !static_cast<const Store *>(this)->isTrulyReducedInShallowReduce())) {
    // We can't beautify unreduced expression
    *simplifiedExpression = e;
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximate<double>(context, complexFormat, angleUnit);
    }
    return;
  }

  // Step 2: we approximate and beautify the reduced expression
  /* Case 1: the reduced expression is a matrix or a list : We scan the
   * children to beautify them with the right complex format. */
  if (e.type() == ExpressionNode::Type::Matrix || e.type() == ExpressionNode::Type::List) {
    SimplifyAndApproximateChildren(e, simplifiedExpression, approximateExpression, context, complexFormat, angleUnit, userReductionContext);
  } else {
    /* Case 2: the reduced expression is scalar or too complex to respect the
     * complex format. */
    e.beautifyAndApproximateScalar(simplifiedExpression, approximateExpression, userReductionContext, context, complexFormat, angleUnit);
  }
}

Expression Expression::ExpressionWithoutSymbols(Expression e, Context * context, ExpressionNode::SymbolicComputation symbolicComputation) {
  if (e.isUninitialized()) {
    return e;
  }
  // Replace all the symbols in depth.
  bool isCircular = false;
  e = e.deepReplaceReplaceableSymbols(context, &isCircular, k_maxSymbolReplacementsCount, 0, symbolicComputation);
  if (!isCircular) {
    return e;
  }
  /* Symbols are defined circularly (or likely to be if we made too many
   * replacements), in which case we return an uninitialized expression. */
  return Expression();
}

Expression Expression::radianToAngleUnit(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*180/Pi
    return Multiplication::Builder(*this, Rational::Builder(180), Power::Builder(Constant::Builder("π"), Rational::Builder(-1)));
  }
  else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*200/Pi
    return Multiplication::Builder(*this, Rational::Builder(200), Power::Builder(Constant::Builder("π"), Rational::Builder(-1)));
  }
  return *this;
}

Expression Expression::angleUnitToRadian(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*Pi/180
    return Multiplication::Builder(*this, Rational::Builder(1, 180), Constant::Builder("π"));
  }
  else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*Pi/200
    return Multiplication::Builder(*this, Rational::Builder(1, 200), Constant::Builder("π"));
  }
  return *this;
}

Expression Expression::reduceAndRemoveUnit(ExpressionNode::ReductionContext reductionContext, Expression * Unit) {
  /* RemoveUnit has to be called on reduced expression. reduce method is called
   * instead of deepReduce to catch interrupted simplification. */
  return deepReduce(reductionContext).removeUnit(Unit);
}

Expression Expression::cloneAndDeepReduceWithSystemCheckpoint(ExpressionNode::ReductionContext * reductionContext, bool * reduceFailure) const {
  *reduceFailure = false;
#if __EMSCRIPTEN__
  Expression e = clone().deepReduce(*reductionContext);
  {
    if (ExceptionCheckpoint::HasBeenInterrupted()) {
      ExceptionCheckpoint::ClearInterruption();
      if (reductionContext->target() == ExpressionNode::ReductionTarget::SystemForApproximation) {
        goto failure;
      }
      reductionContext->setTarget(ExpressionNode::ReductionTarget::SystemForApproximation);
      e = clone().deepReduce(*reductionContext);
      if (ExceptionCheckpoint::HasBeenInterrupted()) {
        ExceptionCheckpoint::ClearInterruption();
failure:
        *reduceFailure = true;
      }
    }
  }
#else
  Expression e;
  {
    char * treePoolCursor = TreePool::sharedPool()->cursor();
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      e = clone().deepReduce(*reductionContext);
    } else {
      /* We don't want to tidy all the Pool in the case we are in a nested
       * cloneAndDeepReduceWithSystemCheckpoint: cleaning all the pool might
       * discard ExpressionHandles that are used by parent
       * cloneAndDeepReduceWithSystemCheckpoint. */
      reductionContext->context()->tidyDownstreamPoolFrom(treePoolCursor);
      if (reductionContext->target() != ExpressionNode::ReductionTarget::SystemForApproximation) {
        // System interruption, try again with another ReductionTarget
        reductionContext->setTarget(ExpressionNode::ReductionTarget::SystemForApproximation);
        e = clone().deepReduce(*reductionContext);
      } else {
        *reduceFailure = true;
      }
    }
  }
#endif
  if (*reduceFailure) {
    // Cloning outside of ecp's scope in case it raises an exception
    e = clone();
  }
  assert(!e.isUninitialized());
  return e;
}

Expression Expression::cloneAndReduce(ExpressionNode::ReductionContext reductionContext) const {
  // TODO: Ensure all cloneAndReduce usages handle reduction failure.
  bool reduceFailure;
  return cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
}

Expression Expression::deepReduce(ExpressionNode::ReductionContext reductionContext) {
  deepReduceChildren(reductionContext);
  if (type() != ExpressionNode::Type::Store && !ComparisonOperator::IsComparisonOperatorType(type())) {
    /* Bubble up dependencies */
    List dependencies = List::Builder();
    for (int i = 0; i < numberOfChildren(); i++) {
      if (isParameteredExpression() && (i == ParameteredExpression::ParameteredChildIndex())) {
        /* A parametered expression can have dependencies on its parameter, which
         * we don't want to factor, as the parameter does not have meaning
         * outside of the parametered expression.
         * The parametered expression will have to handle dependencies manually
         * in its shallowReduce. */
        continue;
      }
      Expression child = childAtIndex(i);
      if (child.type() == ExpressionNode::Type::Dependency) {
        static_cast<Dependency &>(child).extractDependencies(dependencies);
      }
    }
    if (dependencies.numberOfChildren() > 0) {
      Expression e = shallowReduce(reductionContext);
      Expression d = Dependency::Builder(Undefined::Builder(), dependencies);
      e.replaceWithInPlace(d);
      d.replaceChildAtIndexInPlace(0, e);
      if (e.type() == ExpressionNode::Type::Dependency) {
        static_cast<Dependency &>(e).extractDependencies(dependencies);
      }
      return d.shallowReduce(reductionContext);
    }
  }

  return shallowReduce(reductionContext);
}

Expression Expression::defaultDeepBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression e = shallowBeautify(&reductionContext);
  SimplificationHelper::deepBeautifyChildren(e, reductionContext);
  return e;
}

Expression Expression::setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive || s == ExpressionNode::Sign::Negative);
  if (isNumber()) {
    // Needed to avoid infinite loop in Multiplication::shallowReduce
    Number thisNumber = static_cast<Number &>(*this);
    return thisNumber.setSign(s);
  }
  ExpressionNode::Sign currentSign = sign(reductionContext.context());
  assert(currentSign != ExpressionNode::Sign::Unknown);
  if (currentSign == s) {
    return *this;
  }
  Multiplication revertedSign = Multiplication::Builder(Rational::Builder(-1));
  replaceWithInPlace(revertedSign);
  revertedSign.addChildAtIndexInPlace(*this, 1, 1);
  return revertedSign.shallowReduce(reductionContext);
}

int Expression::lengthOfListChildren() const {
  int lastLength = k_noList;
  int n = numberOfChildren();
  bool isNAry = IsNAry(*this, nullptr);
  for (int i = n - 1; i >= 0; i--) {
    if (isNAry && childAtIndex(i).type() < ExpressionNode::Type::List) {
      return lastLength;
    }
    if (childAtIndex(i).type() == ExpressionNode::Type::List) {
      int length = childAtIndex(i).numberOfChildren();
      if (lastLength == k_noList) {
        lastLength = length;
      } else if (lastLength != length) {
        return k_mismatchedLists;
      }
    }
  }
  return lastLength;
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
          imag = Constant::Builder("i");
        } else {
          imag = Multiplication::Builder(tb, Constant::Builder("i"));
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
          arg = Constant::Builder("i");
        } else {
          arg = Multiplication::Builder(tb, Constant::Builder("i"));
        }
        if (isNegativeTb) {
          arg = Opposite::Builder(arg);
        }
        arg.shallowAddMissingParenthesis();
        exp = Power::Builder(Constant::Builder("e"), arg);
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

Coordinate2D<double> Expression::nextMinimum(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, double relativePrecision, double minimalStep, double maximalStep) const {
  const void * pack[] = { this, symbol, &complexFormat, &angleUnit };
  Solver::ValueAtAbscissa evaluation = [](double x, Context * ctx, const void * aux) {
    const void * const * pack = static_cast<const void * const *>(aux);
    const Expression * expr = static_cast<const Expression *>(pack[0]);
    const char * sym = static_cast<const char *>(pack[1]);
    Preferences::ComplexFormat complexFormat = *static_cast<const Preferences::ComplexFormat *>(pack[2]);
    Preferences::AngleUnit angleUnit = *static_cast<const Preferences::AngleUnit *>(pack[3]);
    return expr->approximateWithValueForSymbol(sym, x, ctx, complexFormat, angleUnit);
  };
  return Solver::NextMinimum(evaluation, context, pack, start, max, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> Expression::nextMaximum(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, double relativePrecision, double minimalStep, double maximalStep) const {
  const void * pack[] = { this, symbol, &complexFormat, &angleUnit };
  Solver::ValueAtAbscissa evaluation = [](double x, Context * ctx, const void * aux) {
    const void * const * pack = static_cast<const void * const *>(aux);
    const Expression * expr = static_cast<const Expression *>(pack[0]);
    const char * sym = static_cast<const char *>(pack[1]);
    Preferences::ComplexFormat complexFormat = *static_cast<const Preferences::ComplexFormat *>(pack[2]);
    Preferences::AngleUnit angleUnit = *static_cast<const Preferences::AngleUnit *>(pack[3]);
    return -expr->approximateWithValueForSymbol(sym, x, ctx, complexFormat, angleUnit);
  };
  Coordinate2D<double> result = Solver::NextMinimum(evaluation, context, pack, start, max, relativePrecision, minimalStep, maximalStep);
  return Coordinate2D<double>(result.x1(), -result.x2());
}

double Expression::nextRoot(const char * symbol, double start, double max, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, double relativePrecision, double minimalStep, double maximalStep) const {
  /* The algorithms used to numerically find roots require either the function
   * to change sign around the root or for the root to be an extremum. Neither
   * is true for the null function, which we handle here. */
  if (nullStatus(context) == ExpressionNode::NullStatus::Null) {
    return start + std::copysign(maximalStep, max - start);
  }
  if (type() == ExpressionNode::Type::Power || type() == ExpressionNode::Type::NthRoot || type() == ExpressionNode::Type::SquareRoot) {
    if ((type() == ExpressionNode::Type::Power || type() == ExpressionNode::Type::NthRoot) && childAtIndex(1).sign(context) == ExpressionNode::Sign::Negative) {
      // Powers at negative index can't be null
      return NAN;
    }
    /* A power such as sqrt(x) can have a vertical derivative around its root,
     * making the tolerance used for finding zeroes ill-suited. As such, we
     * make use of the fact that the base of the power needs to be null for the
     * root to be null. */
    double result = childAtIndex(0).nextRoot(symbol, start, max, context, complexFormat, angleUnit, relativePrecision, minimalStep, maximalStep);
    if (std::isnan(result) || (result < start && max > result) || (result > start && max < result)) {
      // result is Nan or out of bounds
      return NAN;
    }
    double exponent = type() == ExpressionNode::Type::SquareRoot ? 0.5 : childAtIndex(1).approximateWithValueForSymbol(symbol, result, context, complexFormat, angleUnit);
    if (!std::isnan(exponent) && exponent > relativePrecision * Solver::k_zeroPrecision) {
      double valueAtRoot = approximateWithValueForSymbol(symbol, result, context, complexFormat, angleUnit);
      if (std::isfinite(valueAtRoot) && std::fabs(valueAtRoot) <= std::pow(relativePrecision * Solver::k_zeroPrecision, exponent)) {
        return result;
      }
    }
    // result should be between max and start
    assert((result >= start && max >= result) || (result <= start && max <= result));
    return nextRoot(symbol, result, max, context, complexFormat, angleUnit, relativePrecision, minimalStep, maximalStep);
  }
  const void * pack[] = { this, symbol, &complexFormat, &angleUnit };
  Solver::ValueAtAbscissa evaluation = [](double x, Context * ctx, const void * aux) {
    const void * const * pack = static_cast<const void * const *>(aux);
    const Expression * expr = static_cast<const Expression *>(pack[0]);
    const char * sym = static_cast<const char *>(pack[1]);
    Preferences::ComplexFormat complexFormat = *static_cast<const Preferences::ComplexFormat *>(pack[2]);
    Preferences::AngleUnit angleUnit = *static_cast<const Preferences::AngleUnit *>(pack[3]);
    return expr->approximateWithValueForSymbol(sym, x, ctx, complexFormat, angleUnit);
  };
  return Solver::NextRoot(evaluation, context, pack, start, max, relativePrecision, minimalStep, maximalStep);
}

Coordinate2D<double> Expression::nextIntersection(const char * symbol, double start, double max, Poincare::Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const Expression expression, double relativePrecision, double minimalStep, double maximalStep) const {
  const void * pack[] = { this, &expression, symbol, &complexFormat, &angleUnit };
  Solver::ValueAtAbscissa evaluation = [](double x, Context * ctx, const void * aux) {
    const void * const * pack = static_cast<const void * const *>(aux);
    const Expression * expr = static_cast<const Expression *>(pack[0]);
    const Expression * expr2 = static_cast<const Expression *>(pack[1]);
    const char * sym = static_cast<const char *>(pack[2]);
    Preferences::ComplexFormat complexFormat = *static_cast<const Preferences::ComplexFormat *>(pack[3]);
    Preferences::AngleUnit angleUnit = *static_cast<const Preferences::AngleUnit *>(pack[4]);
    return expr->approximateWithValueForSymbol(sym, x, ctx, complexFormat, angleUnit) - expr2->approximateWithValueForSymbol(sym, x, ctx, complexFormat, angleUnit);
  };
  double resultX = Solver::NextRoot(evaluation, context, pack, start, max, relativePrecision, minimalStep, maximalStep);
  return Coordinate2D<double>(resultX, approximateWithValueForSymbol(symbol, resultX, context, complexFormat, angleUnit));
}

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
