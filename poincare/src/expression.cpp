#include <poincare/expression.h>
#include <poincare/expression_node.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/undefined.h>
#include <poincare/symbol.h>
#include <poincare/variable_context.h>
#include <ion.h>
#include <cmath>
#include <float.h>

#include "parsing/parser.h"

namespace Poincare {

bool Expression::sSymbolReplacementsCountLock = false;

/* Constructor & Destructor */

Expression Expression::clone() const { TreeHandle c = TreeHandle::clone(); return static_cast<Expression&>(c); }

Expression Expression::Parse(char const * string) {
  if (string[0] == 0) {
    return Expression();
  }
  Parser p(string);
  Expression expression = p.parse();
  if (p.getStatus() != Parser::Status::Success) {
    expression = Expression();
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

void Expression::setCircuitBreaker(CircuitBreaker cb) {
  sCircuitBreaker = cb;
}

bool Expression::shouldStopProcessing() {
  if (sCircuitBreaker == nullptr) {
    return false;
  }
  if (sCircuitBreaker()) {
    sSimplificationHasBeenInterrupted = true;
    return true;
  }
  return false;
}

void Expression::resetInterruption() {
  sSimplificationHasBeenInterrupted = false;
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

bool Expression::recursivelyMatches(ExpressionTest test, Context & context, bool replaceSymbols) const {
  if (test(*this, context, replaceSymbols)) {
    return true;
  }
  for (int i = 0; i < this->numberOfChildren(); i++) {
    if (childAtIndex(i).recursivelyMatches(test, context, replaceSymbols)) {
      return true;
    }
  }
  return false;
}

bool Expression::isApproximate(Context & context) const {
  return recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) {
      return e.type() == ExpressionNode::Type::Decimal
      || e.type() == ExpressionNode::Type::Float
        || IsMatrix(e, context, replaceSymbols)
        || ((e.type() == ExpressionNode::Type::Symbol || e.type() == ExpressionNode::Type::Function)
            && replaceSymbols
            && SymbolAbstract::matches(
                static_cast<const SymbolAbstract&>(e),
                [](const Expression e, Context & context, bool replaceSymbols) {
                  return e.isApproximate(context);
                },
                context));
        }, context, true);
}

bool Expression::IsMatrix(const Expression e, Context & context, bool replaceSymbols) {
  return e.type() == ExpressionNode::Type::Matrix
    || e.type() == ExpressionNode::Type::ConfidenceInterval
    || e.type() == ExpressionNode::Type::MatrixDimension
    || e.type() == ExpressionNode::Type::PredictionInterval
    || e.type() == ExpressionNode::Type::MatrixInverse
    || e.type() == ExpressionNode::Type::MatrixTranspose
    || ((e.type() == ExpressionNode::Type::Symbol || e.type() == ExpressionNode::Type::Function)
        && replaceSymbols
        && SymbolAbstract::matches(
            static_cast<const Symbol&>(e),
            [](const Expression e, Context & context, bool replaceSymbols) {
              return e.recursivelyMatches(
                  [](const Expression e, Context & context, bool replaceSymbols) {
                    return Expression::IsMatrix(e, context, replaceSymbols);
                  },
                  context, replaceSymbols);
            },
            context));
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

bool Expression::getLinearCoefficients(char * variables, int maxVariableSize, Expression coefficients[], Expression constant[], Context & context, Preferences::AngleUnit angleUnit) const {
  // Reset interrupting flag because we use deepReduce
  sSimplificationHasBeenInterrupted = false;
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
    int degree = equation.getPolynomialReducedCoefficients(&variables[index*maxVariableSize], polynomialCoefficients, context, angleUnit);
    switch (degree) {
      case 0:
        coefficients[index] = Rational(0);
        break;
      case 1:
        coefficients[index] = polynomialCoefficients[1];
        break;
      default:
        /* degree is supposed to be 0 or 1. Otherwise, it means that equation
         * is 'undefined' due to the reduction of 0*inf for example.
         * (ie, x*y*inf = 0) */
        assert(!recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) { return e.type() == ExpressionNode::Type::Undefined; }, context, true));
        return false;
    }
    /* The equation is can be written: a_1*x+a_0 with a_1 and a_0 x-independent.
     * The equation supposed to be linear in all variables, so we can look for
     * the coefficients linked to the other variables in a_0. */
    equation = polynomialCoefficients[0];
    index++;
  }
  constant[0] = Opposite(equation.clone()).deepReduce(context, angleUnit, ExpressionNode::ReductionTarget::TopDownComputation);
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

void Expression::defaultDeepReduceChildren(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  for (int i = 0; i < numberOfChildren(); i++) {
    childAtIndex(i).deepReduce(context, angleUnit, target);
  }
}

Expression Expression::defaultShallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  for (int i = 0; i < numberOfChildren(); i++) {
    if (childAtIndex(i).type() == ExpressionNode::Type::Undefined) {
      Expression result = Undefined();
      replaceWithInPlace(result);
      return result;
    }
  }
  return *this;
}

Expression Expression::parent() const {
  TreeHandle p = TreeHandle::parent();
  return static_cast<Expression &>(p);
}

void Expression::defaultSetChildrenInPlace(Expression other) {
  assert(numberOfChildren() == other.numberOfChildren());
  for (int i = 0; i < numberOfChildren(); i++) {
    replaceChildAtIndexInPlace(i, other.childAtIndex(i));
  }
}

bool Expression::hasReplaceableSymbols(Context & context) const {
  return recursivelyMatches([](const Expression e, Context & context, bool replaceSymbols) {
      return (e.type() == ExpressionNode::Type::Symbol
          && !static_cast<const Symbol &>(e).isSystemSymbol()
          && !context.expressionForSymbol(static_cast<const Symbol &>(e), false).isUninitialized())
      || (e.type() == ExpressionNode::Type::Function
           && !context.expressionForSymbol(static_cast<const Function &>(e), false).isUninitialized());
      }, context, false);
}

Expression Expression::defaultReplaceReplaceableSymbols(Context & context) {
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    childAtIndex(i).shallowReplaceReplaceableSymbols(context);
  }
  return *this;
}

template<typename U>
Evaluation<U> Expression::approximateToEvaluation(Context& context, Preferences::AngleUnit angleUnit) const {
  // Reset interrupting flag because some evaluation methods use it
  sSimplificationHasBeenInterrupted = false;
  return node()->approximate(U(), context, angleUnit);
}

Expression Expression::defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression) {
  for (int i = 0; i < numberOfChildren(); i++) {
    childAtIndex(i).replaceSymbolWithExpression(symbol, expression);
  }
  return *this;
}

int Expression::defaultGetPolynomialCoefficients(Context & context, const char * symbol, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbol);
  if (deg == 0) {
    coefficients[0] = clone();
    return 0;
  }
  return -1;
}

int Expression::getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context & context, Preferences::AngleUnit angleUnit) const {
  // Reset interrupting flag because we use deepReduce
  sSimplificationHasBeenInterrupted = false;
  int degree = getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].deepReduce(context, angleUnit, ExpressionNode::ReductionTarget::TopDownComputation);
  }
  return degree;
}

/* Comparison */

bool Expression::isIdenticalTo(const Expression e) const {
  /* We use the simplification order only because it is a already-coded total
   * order on expresssions. */
  return ExpressionNode::SimplificationOrder(node(), e.node(), true) == 0;
}

bool Expression::isEqualToItsApproximationLayout(Expression approximation, char * buffer, int bufferSize, Preferences::AngleUnit angleUnit, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context & context) {
  approximation.serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  /* Warning: we cannot use directly the the approximate expression but we have
   * to re-serialize it because the number of stored significative
   * numbers and the number of displayed significative numbers might not be
   * identical. (For example, 0.000025 might be displayed "0.00003" and stored
   * as Decimal(0.000025) and isEqualToItsApproximationLayout should return
   * false) */
  Expression approximateOutput = Expression::ParseAndSimplify(buffer, context, angleUnit);
  bool equal = isIdenticalTo(approximateOutput);
  return equal;
}

/* Layout Helper */

Layout Expression::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return isUninitialized() ? Layout() : node()->createLayout(floatDisplayMode, numberOfSignificantDigits);
}

int Expression::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return isUninitialized() ? 0 : node()->serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits); }

/* Simplification */

Expression Expression::ParseAndReduce(const char * text, Context & context, Preferences::AngleUnit angleUnit) {
  Expression exp = Parse(text);
  if (exp.isUninitialized()) {
    return Undefined();
  }
  return exp.reduce(context, angleUnit);
}

Expression Expression::ParseAndSimplify(const char * text, Context & context, Preferences::AngleUnit angleUnit) {
  Expression exp = Parse(text);
  if (exp.isUninitialized()) {
    return Undefined();
  }
  exp = exp.simplify(context, angleUnit);
  /* simplify might have been interrupted, in which case the resulting
   * expression is uninitialized, so we need to check that. */
  if (exp.isUninitialized()) {
    return Parse(text);
  }
  return exp;
}

Expression Expression::simplify(Context & context, Preferences::AngleUnit angleUnit) {
  sSimplificationHasBeenInterrupted = false;
  Expression e = deepReduce(context, angleUnit, ExpressionNode::ReductionTarget::User);
  if (!sSimplificationHasBeenInterrupted) {
    e = e.deepBeautify(context, angleUnit);
  }
  return sSimplificationHasBeenInterrupted ? Expression() : e;
}

Expression Expression::ExpressionWithoutSymbols(Expression e, Context & context) {
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

Expression Expression::reduce(Context & context, Preferences::AngleUnit angleUnit) {
  sSimplificationHasBeenInterrupted = false;
  return deepReduce(context, angleUnit, ExpressionNode::ReductionTarget::TopDownComputation);
}

Expression Expression::deepReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
#if MATRIX_EXACT_REDUCING
#else
  if (IsMatrix(*this, context, true)) {
    sSimplificationHasBeenInterrupted = true;
    return *this;
  }
#endif

  deepReduceChildren(context, angleUnit, target);
  if (sSimplificationHasBeenInterrupted) {
    return *this;
  }
  return shallowReduce(context, angleUnit, target);
}

Expression Expression::deepBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = shallowBeautify(context, angleUnit);
  int nbChildren = e.numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    e.childAtIndex(i).deepBeautify(context, angleUnit);
  }
  return e;
}

Expression Expression::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return node()->setSign(s, context, angleUnit);
}

/* Evaluation */

template<typename U>
Expression Expression::approximate(Context& context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat) const {
  return isUninitialized() ? Undefined() : approximateToEvaluation<U>(context, angleUnit).complexToExpression(complexFormat);
}


template<typename U>
U Expression::approximateToScalar(Context& context, Preferences::AngleUnit angleUnit) const {
  return approximateToEvaluation<U>(context, angleUnit).toScalar();
}

template<typename U>
U Expression::approximateToScalar(const char * text, Context& context, Preferences::AngleUnit angleUnit) {
  Expression exp = ParseAndReduce(text, context, angleUnit);
  return exp.approximateToScalar<U>(context, angleUnit);
}

template<typename U>
U Expression::approximateWithValueForSymbol(const char * symbol, U x, Context & context, Preferences::AngleUnit angleUnit) const {
  VariableContext variableContext = VariableContext(symbol, &context);
  variableContext.setApproximationForVariable<U>(x);
  return approximateToScalar<U>(variableContext, angleUnit);
}

template<typename U>
U Expression::epsilon() {
  static U epsilon = sizeof(U) == sizeof(double) ? 1E-15 : 1E-7f;
  return epsilon;
}

/* Expression roots/extrema solver*/

typename Expression::Coordinate2D Expression::nextMinimum(const char * symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const {
  return nextMinimumOfExpression(symbol, start, step, max, [](const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1 = Expression()) {
        return expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit);
      }, context, angleUnit);
}

typename Expression::Coordinate2D Expression::nextMaximum(const char * symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const {
  Coordinate2D minimumOfOpposite = nextMinimumOfExpression(symbol, start, step, max, [](const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1 = Expression()) {
        return -expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit);
      }, context, angleUnit);
  return {.abscissa = minimumOfOpposite.abscissa, .value = -minimumOfOpposite.value};
}

double Expression::nextRoot(const char * symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const {
  return nextIntersectionWithExpression(symbol, start, step, max, [](const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1 = Expression()) {
        return expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit);
      }, context, angleUnit, nullptr);
}

typename Expression::Coordinate2D Expression::nextIntersection(const char * symbol, double start, double step, double max, Poincare::Context & context, Preferences::AngleUnit angleUnit, const Expression expression) const {
  double resultAbscissa = nextIntersectionWithExpression(symbol, start, step, max, [](const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1) {
        return expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit)-expression1.approximateWithValueForSymbol(symbol, x, context, angleUnit);
      }, context, angleUnit, expression);
  typename Expression::Coordinate2D result = {.abscissa = resultAbscissa, .value = approximateWithValueForSymbol(symbol, resultAbscissa, context, angleUnit)};
  if (std::fabs(result.value) < step*k_solverPrecision) {
    result.value = 0.0;
  }
  return result;
}

typename Expression::Coordinate2D Expression::nextMinimumOfExpression(const char * symbol, double start, double step, double max, EvaluationAtAbscissa evaluate, Context & context, Preferences::AngleUnit angleUnit, const Expression expression, bool lookForRootMinimum) const {
  Coordinate2D result = {.abscissa = NAN, .value = NAN};
  if (start == max || step == 0.0) {
    return result;
  }
  double bracket[3];
  double x = start;
  bool endCondition = false;
  do {
    bracketMinimum(symbol, x, step, max, bracket, evaluate, context, angleUnit, expression);
    result = brentMinimum(symbol, bracket[0], bracket[2], evaluate, context, angleUnit, expression);
    x = bracket[1];
    // Because of float approximation, exact zero is never reached
    if (std::fabs(result.abscissa) < std::fabs(step)*k_solverPrecision) {
      result.abscissa = 0;
      result.value = evaluate(symbol, 0, context, angleUnit, *this, expression);
    }
    /* Ignore extremum whose value is undefined or too big because they are
     * really unlikely to be local extremum. */
    if (std::isnan(result.value) || std::fabs(result.value) > k_maxFloat) {
      result.abscissa = NAN;
    }
    // Idem, exact 0 never reached
    if (std::fabs(result.value) < std::fabs(step)*k_solverPrecision) {
      result.value = 0;
    }
    endCondition = std::isnan(result.abscissa) && (step > 0.0 ? x <= max : x >= max);
    if (lookForRootMinimum) {
      endCondition |= std::fabs(result.value) > 0 && (step > 0.0 ? x <= max : x >= max);
    }
  } while (endCondition);
  if (lookForRootMinimum) {
    result.abscissa = std::fabs(result.value) > 0 ? NAN : result.abscissa;
  }
  return result;
}

void Expression::bracketMinimum(const char * symbol, double start, double step, double max, double result[3], EvaluationAtAbscissa evaluate, Context & context, Preferences::AngleUnit angleUnit, const Expression expression) const {
  Coordinate2D p[3];
  p[0] = {.abscissa = start, .value = evaluate(symbol, start, context, angleUnit, *this, expression)};
  p[1] = {.abscissa = start+step, .value = evaluate(symbol, start+step, context, angleUnit, *this, expression)};
  double x = start+2.0*step;
  while (step > 0.0 ? x <= max : x >= max) {
    p[2] = {.abscissa = x, .value = evaluate(symbol, x, context, angleUnit, *this, expression)};
    if ((p[0].value > p[1].value || std::isnan(p[0].value)) && (p[2].value > p[1].value || std::isnan(p[2].value)) && (!std::isnan(p[0].value) || !std::isnan(p[2].value))) {
      result[0] = p[0].abscissa;
      result[1] = p[1].abscissa;
      result[2] = p[2].abscissa;
      return;
    }
    if (p[0].value > p[1].value && p[1].value == p[2].value) {
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

typename Expression::Coordinate2D Expression::brentMinimum(const char * symbol, double ax, double bx, EvaluationAtAbscissa evaluate, Context & context, Preferences::AngleUnit angleUnit, const Expression expression) const {
  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  if (ax > bx) {
    return brentMinimum(symbol, bx, ax, evaluate, context, angleUnit, expression);
  }
  double e = 0.0;
  double a = ax;
  double b = bx;
  double x = a+k_goldenRatio*(b-a);
  double v = x;
  double w = x;
  double fx = evaluate(symbol, x, context, angleUnit, *this, expression);
  double fw = fx;
  double fv = fw;

  double d = NAN;
  double u, fu;

  for (int i = 0; i < 100; i++) {
    double m = 0.5*(a+b);
    double tol1 = k_sqrtEps*std::fabs(x)+1E-10;
    double tol2 = 2.0*tol1;
    if (std::fabs(x-m) <= tol2-0.5*(b-a))  {
      double middleFax = evaluate(symbol, (x+a)/2.0, context, angleUnit, *this, expression);
      double middleFbx = evaluate(symbol, (x+b)/2.0, context, angleUnit, *this, expression);
      double fa = evaluate(symbol, a, context, angleUnit, *this, expression);
      double fb = evaluate(symbol, b, context, angleUnit, *this, expression);
      if (middleFax-fa <= k_sqrtEps && fx-middleFax <= k_sqrtEps && fx-middleFbx <= k_sqrtEps && middleFbx-fb <= k_sqrtEps) {
        Coordinate2D result = {.abscissa = x, .value = fx};
        return result;
      }
    }
    double p = 0;
    double q = 0;
    double r = 0;
    if (std::fabs(e) > tol1) {
      r = (x-w)*(fx-fv);
      q = (x-v)*(fx-fw);
      p = (x-v)*q -(x-w)*r;
      q = 2.0*(q-r);
      if (q>0.0) {
        p = -p;
      } else {
        q = -q;
      }
      r = e;
      e = d;
    }
    if (std::fabs(p) < std::fabs(0.5*q*r) && p<q*(a-x) && p<q*(b-x)) {
      d = p/q;
      u= x+d;
      if (u-a < tol2 || b-u < tol2) {
        d = x < m ? tol1 : -tol1;
      }
    } else {
      e = x<m ? b-x : a-x;
      d = k_goldenRatio*e;
    }
    u = x + (std::fabs(d) >= tol1 ? d : (d>0 ? tol1 : -tol1));
    fu = evaluate(symbol, u, context, angleUnit, *this, expression);
    if (fu <= fx) {
      if (u<x) {
        b = x;
      } else {
        a = x;
      }
      v = w;
      fv = fw;
      w = x;
      fw = fx;
      x = u;
      fx = fu;
    } else {
      if (u<x) {
        a = u;
      } else {
        b = u;
      }
      if (fu <= fw || w == x) {
        v = w;
        fv = fw;
        w = u;
        fw = fu;
      } else if (fu <= fv || v == x || v == w) {
        v = u;
        fv = fu;
      }
    }
  }
  Coordinate2D result = {.abscissa = x, .value = fx};
  return result;
}

double Expression::nextIntersectionWithExpression(const char * symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression expression) const {
  if (start == max || step == 0.0) {
    return NAN;
  }
  double bracket[2];
  double result = NAN;
  static double precisionByGradUnit = 1E6;
  double x = start+step;
  do {
    bracketRoot(symbol, x, step, max, bracket, evaluation, context, angleUnit, expression);
    result = brentRoot(symbol, bracket[0], bracket[1], std::fabs(step/precisionByGradUnit), evaluation, context, angleUnit, expression);
    x = bracket[1];
  } while (std::isnan(result) && (step > 0.0 ? x <= max : x >= max));

  double extremumMax = std::isnan(result) ? max : result;
  Coordinate2D resultExtremum[2] = {
    nextMinimumOfExpression(symbol, start, step, extremumMax, [](const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1) {
        if (expression1.isUninitialized()) {
          return expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit);
        } else {
          return expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit)-expression1.approximateWithValueForSymbol(symbol, x, context, angleUnit);
        }
      }, context, angleUnit, expression, true),
    nextMinimumOfExpression(symbol, start, step, extremumMax, [](const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit, const Expression expression0, const Expression expression1) {
        if (expression1.isUninitialized()) {
          return -expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit);
        } else {
          return expression1.approximateWithValueForSymbol(symbol, x, context, angleUnit)-expression0.approximateWithValueForSymbol(symbol, x, context, angleUnit);
        }
      }, context, angleUnit, expression, true)};
  for (int i = 0; i < 2; i++) {
    if (!std::isnan(resultExtremum[i].abscissa) && (std::isnan(result) || std::fabs(result - start) > std::fabs(resultExtremum[i].abscissa - start))) {
      result = resultExtremum[i].abscissa;
    }
  }
  if (std::fabs(result) < std::fabs(step)*k_solverPrecision) {
    result = 0;
  }
  return result;
}

void Expression::bracketRoot(const char * symbol, double start, double step, double max, double result[2], EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression expression) const {
  double a = start;
  double b = start+step;
  while (step > 0.0 ? b <= max : b >= max) {
    double fa = evaluation(symbol, a, context, angleUnit, *this, expression);
    double fb = evaluation(symbol, b, context, angleUnit,* this, expression);
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

double Expression::brentRoot(const char * symbol, double ax, double bx, double precision, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const Expression expression) const {
  if (ax > bx) {
    return brentRoot(symbol, bx, ax, precision, evaluation, context, angleUnit, expression);
  }
  double a = ax;
  double b = bx;
  double c = bx;
  double d = b-a;
  double e = b-a;
  double fa = evaluation(symbol, a, context, angleUnit, *this, expression);
  double fb = evaluation(symbol, b, context, angleUnit, *this, expression);
  double fc = fb;
  for (int i = 0; i < 100; i++) {
    if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
      c = a;
      fc = fa;
      e = b-a;
      d = b-a;
    }
    if (std::fabs(fc) < std::fabs(fb)) {
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }
    double tol1 = 2.0*DBL_EPSILON*std::fabs(b)+0.5*precision;
    double xm = 0.5*(c-b);
    if (std::fabs(xm) <= tol1 || fb == 0.0) {
      double fbcMiddle = evaluation(symbol, 0.5*(b+c), context, angleUnit, *this, expression);
      double isContinuous = (fb <= fbcMiddle && fbcMiddle <= fc) || (fc <= fbcMiddle && fbcMiddle <= fb);
      if (isContinuous) {
        return b;
      }
    }
    if (std::fabs(e) >= tol1 && std::fabs(fa) > std::fabs(b)) {
      double s = fb/fa;
      double p = 2.0*xm*s;
      double q = 1.0-s;
      if (a != c) {
        q = fa/fc;
        double r = fb/fc;
        p = s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
        q = (q-1.0)*(r-1.0)*(s-1.0);
      }
      q = p > 0.0 ? -q : q;
      p = std::fabs(p);
      double min1 = 3.0*xm*q-std::fabs(tol1*q);
      double min2 = std::fabs(e*q);
      if (2.0*p < (min1 < min2 ? min1 : min2)) {
        e = d;
        d = p/q;
      } else {
        d = xm;
        e =d;
      }
    } else {
      d = xm;
      e = d;
    }
    a = b;
    fa = fb;
    if (std::fabs(d) > tol1) {
      b += d;
    } else {
      b += xm > 0.0 ? tol1 : tol1;
    }
    fb = evaluation(symbol, b, context, angleUnit, *this, expression);
  }
  return NAN;
}

template float Expression::epsilon<float>();
template double Expression::epsilon<double>();

template Expression Expression::approximate<float>(Context& context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat) const;
template Expression Expression::approximate<double>(Context& context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat) const;

template float Expression::approximateToScalar(Context& context, Preferences::AngleUnit angleUnit) const;
template double Expression::approximateToScalar(Context& context, Preferences::AngleUnit angleUnit) const;

template float Expression::approximateToScalar<float>(const char * text, Context& context, Preferences::AngleUnit angleUnit);
template double Expression::approximateToScalar<double>(const char * text, Context& context, Preferences::AngleUnit angleUnit);

template Evaluation<float> Expression::approximateToEvaluation(Context& context, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> Expression::approximateToEvaluation(Context& context, Preferences::AngleUnit angleUnit) const;

template float Expression::approximateWithValueForSymbol(const char * symbol, float x, Context & context, Preferences::AngleUnit angleUnit) const;
template double Expression::approximateWithValueForSymbol(const char * symbol, double x, Context & context, Preferences::AngleUnit angleUnit) const;

}
