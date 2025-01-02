#include <float.h>
#include <ion.h>
#include <omg/float.h>
#include <poincare/helpers/symbol.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/based_integer.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/constant.h>
#include <poincare/old/decimal.h>
#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/expression_node.h>
#include <poincare/old/float.h>
#include <poincare/old/ghost.h>
#include <poincare/old/imaginary_part.h>
#include <poincare/old/list.h>
#include <poincare/old/matrix.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/number.h>
#include <poincare/old/old_expression.h>
#include <poincare/old/opposite.h>
#include <poincare/old/parenthesis.h>
#include <poincare/old/piecewise_operator.h>
#include <poincare/old/point_evaluation.h>
#include <poincare/old/pool_variable_context.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/real_part.h>
#include <poincare/old/store.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/old/unit.h>
#include <poincare/src/layout/layouter.h>

#include <cmath>
#include <utility>

namespace Poincare {

static bool s_approximationEncounteredComplex = false;
static bool s_reductionEncounteredUndistributedList = false;

/* Constructor & Destructor */

OExpression OExpression::clone() const {
  assert(false);
  return OExpression();
}

OExpression OExpression::ExpressionFromAddress(const void *address,
                                               size_t size) {
  if (address == nullptr || size == 0) {
    return OExpression();
  }
  // Build the OExpression in the Tree Pool
  return OExpression(static_cast<ExpressionNode *>(
      Pool::sharedPool->copyTreeFromAddress(address, size)));
}

/* Hierarchy */

OExpression OExpression::childAtIndex(int i) const {
  assert(i >= 0 && i < numberOfChildren());
  PoolHandle c = PoolHandle::childAtIndex(i);
  return static_cast<OExpression &>(c);
}

/* Properties */

bool OExpression::isZero() const {
  return isNumber() && convert<const Number>().isZero();
}

bool OExpression::isRationalOne() const {
  return otype() == ExpressionNode::Type::Rational && isOne();
}

bool OExpression::isOne() const {
  return isNumber() && convert<const Number>().isOne();
}

bool OExpression::isMinusOne() const {
  return (isNumber() && convert<const Number>().isMinusOne()) ||
         (otype() == ExpressionNode::Type::Opposite && childAtIndex(0).isOne());
}

bool OExpression::isInteger() const {
  return (isNumber() && convert<const Number>().isInteger()) ||
         (otype() == ExpressionNode::Type::Opposite &&
          childAtIndex(0).isInteger());
}

bool OExpression::recursivelyMatches(ExpressionTrinaryTest test,
                                     Context *context,
                                     SymbolicComputation replaceSymbols,
                                     void *auxiliary,
                                     IgnoredSymbols *ignoredSymbols) const {
  return false;
}

bool OExpression::recursivelyMatches(ExpressionTest test, Context *context,
                                     SymbolicComputation replaceSymbols) const {
  return false;
}

bool OExpression::recursivelyMatches(SimpleExpressionTest test,
                                     Context *context,
                                     SymbolicComputation replaceSymbols) const {
  return false;
}

bool OExpression::recursivelyMatches(ExpressionTestAuxiliary test,
                                     Context *context,
                                     SymbolicComputation replaceSymbols,
                                     void *auxiliary) const {
  return false;
}

bool OExpression::deepIsOfType(
    std::initializer_list<ExpressionNode::Type> types, Context *context) const {
  return recursivelyMatches(
      [](const OExpression e, Context *context, void *auxiliary) {
        return e.isOfType(
                   *static_cast<std::initializer_list<ExpressionNode::Type> *>(
                       auxiliary))
                   ? OMG::Troolean::True
                   : OMG::Troolean::Unknown;
      },
      context, SymbolicComputation::ReplaceDefinedSymbols, &types);
}

bool OExpression::deepIsMatrix(Context *context, bool canContainMatrices,
                               bool isReduced) const {
  if (!canContainMatrices) {
    return false;
  }
  return recursivelyMatches(
      [](const OExpression e, Context *context, void *auxiliary) {
        if (IsMatrix(e, context)) {
          return OMG::Troolean::True;
        }
        if (IsNAry(e) && e.numberOfChildren() > 0) {
          bool *isReduced = static_cast<bool *>(auxiliary);
          /* If reduction didn't fail, the children were sorted so any
           * expression which is a matrix (deeply) would be at the end.
           * Otherwise, it could be at any index. */
          int firstChildToCheck = e.numberOfChildren() - 1;
          int lastChildToCheck = *isReduced ? firstChildToCheck : 0;
          for (int i = firstChildToCheck; i >= lastChildToCheck; i--) {
            if (e.childAtIndex(i).deepIsMatrix(context)) {
              return OMG::Troolean::True;
            }
          }
        }
        /* These types are matrices if one of their children is one.
         * For dependencies, we will check only the first child thanks
         * to dependencyRecursivelyMatches.*/
        /* A point should be undef on matrix but we still need add a check here
         * because reduction can fail. */
        if (e.isOfType(
                {ExpressionNode::Type::Power, ExpressionNode::Type::Opposite,
                 ExpressionNode::Type::Sum, ExpressionNode::Type::Product,
                 ExpressionNode::Type::Dependency, ExpressionNode::Type::Point,
                 ExpressionNode::Type::PiecewiseOperator})) {
          return OMG::Troolean::Unknown;
        }
        // Any other type is not a matrix
        return OMG::Troolean::False;
      },
      context, SymbolicComputation::ReplaceDefinedSymbols, &isReduced);
}

bool OExpression::deepIsList(Context *context) const {
  return recursivelyMatches(
      [](const OExpression e, Context *context, void *) {
        switch (e.otype()) {
          /* These expressions are always lists. */
          case ExpressionNode::Type::OList:
          case ExpressionNode::Type::ListElement:
          case ExpressionNode::Type::ListSlice:
          case ExpressionNode::Type::ListSequence:
          case ExpressionNode::Type::ListSort:
          case ExpressionNode::Type::RandintNoRepeat:
            return OMG::Troolean::True;

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
            return OMG::Troolean::False;

          /* Other expressions may be lists if their children are lists. */
          default:
            return OMG::Troolean::Unknown;
        }
      },
      context);
}

bool OExpression::IsRandom(const OExpression e) { return e.isRandom(); }

bool OExpression::IsMatrix(const OExpression e, Context *context) {
  return e.otype() == ExpressionNode::Type::OMatrix
         /* A Dimension is a matrix unless its child is a list. */
         || (e.otype() == ExpressionNode::Type::Dimension &&
             !e.childAtIndex(0).deepIsList(context)) ||
         e.isOfType({ExpressionNode::Type::MatrixInverse,
                     ExpressionNode::Type::MatrixIdentity,
                     ExpressionNode::Type::MatrixTranspose,
                     ExpressionNode::Type::MatrixRowEchelonForm,
                     ExpressionNode::Type::MatrixReducedRowEchelonForm,
                     ExpressionNode::Type::VectorCross});
}

bool OExpression::IsDiscontinuous(const OExpression e, Context *context) {
  return e.isRandom() || e.otype() == ExpressionNode::Type::PiecewiseOperator ||
         (e.isOfType({ExpressionNode::Type::Floor, ExpressionNode::Type::Round,
                      ExpressionNode::Type::Ceiling,
                      ExpressionNode::Type::FracPart,
                      ExpressionNode::Type::AbsoluteValue}) &&
          e.deepIsOfType({ExpressionNode::Type::Symbol}, context));
}

bool OExpression::deepIsSymbolic(Context *context,
                                 SymbolicComputation replaceSymbols) const {
  return recursivelyMatches(IsSymbolic, context, replaceSymbols);
}

bool OExpression::IsSymbolic(const OExpression e) {
  return e.isOfType({ExpressionNode::Type::Symbol,
                     ExpressionNode::Type::Function,
                     ExpressionNode::Type::Sequence});
}

bool containsVariables(const OExpression e, char *variables,
                       int maxVariableSize) {
  if (e.otype() == ExpressionNode::Type::Symbol) {
    int index = 0;
    while (variables[index * maxVariableSize] != 0) {
      if (strcmp(static_cast<const Symbol &>(e).name(),
                 &variables[index * maxVariableSize]) == 0) {
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

bool OExpression::getLinearCoefficients(
    char *variables, int maxVariableSize, OExpression coefficients[],
    OExpression *constant, Context *context,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat,
    SymbolicComputation symbolicComputation) const {
  assert(!recursivelyMatches(IsMatrix, context, symbolicComputation));
  // variables is in fact of type char[k_maxNumberOfVariables][maxVariableSize]
  int numberOfVariables = 0;
  while (numberOfVariables < k_maxNumberOfVariables &&
         variables[numberOfVariables * maxVariableSize] != 0) {
    int degree = polynomialDegree(
        context, &variables[numberOfVariables * maxVariableSize]);
    if (degree > 1 || degree < 0) {
      return false;
    }
    numberOfVariables++;
  }
  OExpression equation = *this;
  OExpression polynomialCoefficients[k_maxNumberOfPolynomialCoefficients];
  for (int index = 0; index < numberOfVariables; index++) {
    assert(variables[index * maxVariableSize] != 0);
    int degree = equation.getPolynomialReducedCoefficients(
        &variables[index * maxVariableSize], polynomialCoefficients, context,
        complexFormat, angleUnit, unitFormat, symbolicComputation);
    switch (degree) {
      case 0:
        coefficients[index] = Rational::Builder(0);
        break;
      case 1:
        coefficients[index] = polynomialCoefficients[1];
        break;
      default:
        /* Degree is supposed to be 0 or 1. Otherwise, it means that equation
         * is 'undefined' due to the reduction of 0*inf for example.
         * (ie, x*y*inf = 0) */
        assert(!recursivelyMatches(OExpression::IsUndefined, context,
                                   SymbolicComputation::KeepAllSymbols));
        /* Maybe here we would like to return another error than
         * Error::NonLinearSystem, maybe it would be better to return
         * Error::EquationUndefined */
        return false;
    }
    /* The equation can be written: a_1*x+a_0 with a_1 and a_0 x-independent.
     * The equation supposed to be linear in all variables, so we can look for
     * the coefficients linked to the other variables in a_0. */
    equation = polynomialCoefficients[0];
  }
  *constant = Opposite::Builder(equation).cloneAndReduce(ReductionContext(
      context, complexFormat, angleUnit, unitFormat,
      ReductionTarget::SystemForApproximation, symbolicComputation));
  /* The expression can be linear on all coefficients taken one by one but
   * non-linear (ex: xy = 2). We delete the results and return false if one of
   * the coefficients contains a variable. */
  bool isMultivariablePolynomial =
      containsVariables(*constant, variables, maxVariableSize);
  for (int i = 0; i < numberOfVariables; i++) {
    if (isMultivariablePolynomial) {
      break;
    }
    isMultivariablePolynomial =
        containsVariables(coefficients[i], variables, maxVariableSize);
  }
  return !isMultivariablePolynomial;
}

bool OExpression::allChildrenAreReal(Context *context,
                                     bool canContainMatrices) const {
  int n = numberOfChildren();
  for (int i = 0; i < n; i++) {
    OExpression c = childAtIndex(i);
    if (!c.isReal(context, canContainMatrices)) {
      return false;
    }
  }
  return true;
}

bool OExpression::isAlternativeFormOfRationalNumber() const {
  return isOfType({ExpressionNode::Type::Rational,
                   ExpressionNode::Type::BasedInteger,
                   ExpressionNode::Type::Decimal}) ||
         (otype() == ExpressionNode::Type::Division &&
          childAtIndex(0).isAlternativeFormOfRationalNumber() &&
          childAtIndex(1).isAlternativeFormOfRationalNumber()) ||
         (otype() == ExpressionNode::Type::Opposite &&
          childAtIndex(0).isAlternativeFormOfRationalNumber());
}

bool OExpression::involvesDiscontinuousFunction(Context *context) const {
  return recursivelyMatches(IsDiscontinuous, context);
}

bool OExpression::hasBooleanValue() const {
  return isOfType({ExpressionNode::Type::OBoolean,
                   ExpressionNode::Type::Comparison,
                   ExpressionNode::Type::LogicalOperatorNot,
                   ExpressionNode::Type::BinaryLogicalOperator});
}

bool OExpression::derivate(const ReductionContext &reductionContext,
                           Symbol symbol, OExpression symbolValue) {
  return node()->derivate(reductionContext, symbol, symbolValue);
}

void OExpression::derivateChildAtIndexInPlace(
    int index, const ReductionContext &reductionContext, Symbol symbol,
    OExpression symbolValue) {
  if (!childAtIndex(index).derivate(reductionContext, symbol, symbolValue)) {
    replaceChildAtIndexInPlace(
        index, Derivative::Builder(childAtIndex(index),
                                   symbol.clone().convert<Symbol>(),
                                   symbolValue.clone()));
  }
}

// Private

void OExpression::shallowAddMissingParenthesis() {
  if (isUninitialized()) {
    return;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    OExpression child = childAtIndex(i);
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}

OExpression OExpression::addMissingParentheses() {
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    OExpression child = childAtIndex(i).addMissingParentheses();
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      child = Parenthesis::Builder(child);
    }
    replaceChildAtIndexInPlace(i, child);
  }
  return *this;
}

OExpression OExpression::shallowReduceUsingApproximation(
    const ReductionContext &reductionContext) {
  double approx =
      node()
          ->approximate(double(), ApproximationContext(reductionContext, true))
          .toScalar();
  /* If approx is capped by the largest integer such as all smaller integers can
   * be exactly represented in IEEE754, approx is the exact result (no
   * precision were loss). */
  if (!std::isnan(approx) &&
      std::fabs(approx) <= k_largestExactIEEE754Integer) {
    OExpression result = Decimal::Builder(approx);
    replaceWithInPlace(result);
    result = result.shallowReduce(reductionContext);
    assert(result.otype() == ExpressionNode::Type::Rational);
    return result;
  }
  return *this;
}

OExpression OExpression::parent() const {
  PoolHandle p = PoolHandle::parent();
  return static_cast<OExpression &>(p);
}

OExpression OExpression::replaceWithUndefinedInPlace() {
  OExpression result = Undefined::Builder();
  replaceWithInPlace(result);
  return result;
}

void OExpression::defaultSetChildrenInPlace(OExpression other) {
  const int childrenCount = numberOfChildren();
  assert(childrenCount == other.numberOfChildren());
  for (int i = 0; i < childrenCount; i++) {
    replaceChildAtIndexInPlace(i, other.childAtIndex(i));
  }
}

OExpression OExpression::makePositiveAnyNegativeNumeralFactor(
    const ReductionContext &reductionContext) {
  // The expression is a negative number
  if (isNumber() &&
      isPositive(reductionContext.context()) == OMG::Troolean::False) {
    return setSign(true, reductionContext);
  }
  // The expression is a multiplication whose numeral factor is negative
  if (otype() == ExpressionNode::Type::Multiplication &&
      numberOfChildren() > 0 && childAtIndex(0).isNumber() &&
      childAtIndex(0).isPositive(reductionContext.context()) ==
          OMG::Troolean::False) {
    Multiplication m = convert<Multiplication>();
    if (m.childAtIndex(0).isMinusOne()) {
      // The negative numeral factor is -1, we just remove it
      m.removeChildAtIndexInPlace(0);
      // The multiplication can have only one child after removing -1
      return m.squashUnaryHierarchyInPlace();
    } else {
      // Otherwise, we make it positive
      m.childAtIndex(0).setSign(true, reductionContext);
    }
    return std::move(m);
  }
  return OExpression();
}

OExpression OExpression::deepReplaceSymbolWithExpression(
    const SymbolAbstract &symbol, const OExpression expression) {
  /* In this case, replacing a symbol does not alter the number of children,
   * since no other operation (e.g. reduction) is applied. */
  const int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    assert(nbChildren == numberOfChildren());
    childAtIndex(i).replaceSymbolWithExpression(symbol, expression);
  }
  return *this;
}

int OExpression::defaultGetPolynomialCoefficients(
    int degree, Context *context, const char *symbol,
    OExpression coefficients[]) const {
  if (degree == 0) {
    coefficients[0] = clone();
    return 0;
  }
  assert(degree == -1 || degree >= k_maxNumberOfPolynomialCoefficients);
  return -1;
}

int OExpression::getPolynomialReducedCoefficients(
    const char *symbolName, OExpression coefficients[], Context *context,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation,
    bool keepDependencies) const {
  int degree = getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].cloneAndReduce(ReductionContext(
        context, complexFormat, angleUnit, unitFormat,
        ReductionTarget::SystemForApproximation, symbolicComputation));
    if (!keepDependencies &&
        coefficients[i].otype() == ExpressionNode::Type::Dependency) {
      coefficients[i] = coefficients[i].childAtIndex(0);
    }
  }
  return degree;
}

/* Units */

bool OExpression::hasUnit(bool ignoreAngleUnits, bool *hasAngleUnits,
                          bool replaceSymbols, Context *ctx) const {
  if (hasAngleUnits) {
    *hasAngleUnits = false;
  }
  struct Pack {
    bool ignoreAngleUnits;
    bool *hasAngleUnits;
  };
  Pack pack{ignoreAngleUnits, hasAngleUnits};
  return recursivelyMatches(
      [](const OExpression e, Context *context, void *arg) {
        Pack *pack = static_cast<Pack *>(arg);
        bool isAngleUnit = e.isPureAngleUnit();
        bool *hasAngleUnits = pack->hasAngleUnits;
        if (isAngleUnit && hasAngleUnits) {
          *hasAngleUnits = true;
        }
        return (e.otype() == ExpressionNode::Type::OUnit &&
                (!pack->ignoreAngleUnits || !isAngleUnit)) ||
               e.otype() == ExpressionNode::Type::ConstantPhysics;
      },
      ctx,
      replaceSymbols ? SymbolicComputation::ReplaceDefinedSymbols
                     : SymbolicComputation::KeepAllSymbols,
      &pack);
}

bool OExpression::isPureAngleUnit() const {
  return !isUninitialized() && otype() == ExpressionNode::Type::OUnit &&
         convert<OUnit>().representative()->dimensionVector() ==
             OUnit::AngleRepresentative::Default().dimensionVector();
}

bool OExpression::isInRadians(Context *context) const {
  OExpression units;
  ReductionContext reductionContext;
  reductionContext.setContext(context);
  reductionContext.setUnitConversion(UnitConversion::None);
  OExpression thisClone = cloneAndReduceAndRemoveUnit(reductionContext, &units);
  return !units.isUninitialized() &&
         units.otype() == ExpressionNode::Type::OUnit &&
         units.convert<OUnit>().representative() ==
             &OUnit::k_angleRepresentatives[OUnit::k_radianRepresentativeIndex];
}

/* Complex */

bool OExpression::EncounteredComplex() {
  return s_approximationEncounteredComplex;
}

void OExpression::SetEncounteredComplex(bool encounterComplex) {
  s_approximationEncounteredComplex = encounterComplex;
}

bool OExpression::hasComplexI(Context *context,
                              SymbolicComputation replaceSymbols) const {
  return !isUninitialized() &&
         recursivelyMatches(
             [](const OExpression e, Context *context) {
               return (e.otype() == ExpressionNode::Type::ConstantMaths &&
                       static_cast<const Constant &>(e).isComplexI()) ||
                      (e.otype() == ExpressionNode::Type::ComplexCartesian &&
                       static_cast<const ComplexCartesian &>(e).imag().isNull(
                           context) != OMG::Troolean::True);
             },
             context, replaceSymbols);
}

bool OExpression::isReal(Context *context, bool canContainMatrices) const {
  /* We could do something virtual instead of implementing a disjunction on
   * types but many expressions have the same implementation so it is easier to
   * factorize it here. */

  // These expressions are always real
  if ((isNumber() && !isUndefined()) ||
      otype() == ExpressionNode::Type::Random) {
    return true;
  }

  /* These expressions are real when they do not contain a matrix or a list
   * - either they always return a real value on any complex
   *   (AbsoluteValue, ComplexArgument, ImaginaryPart, RealPart)
   * - or they return undef on complex
   *   (all the others) */
  if (isOfType({
          ExpressionNode::Type::AbsoluteValue,
          ExpressionNode::Type::BinomialCoefficient,
          ExpressionNode::Type::Ceiling,
          ExpressionNode::Type::ComplexArgument,
          ExpressionNode::Type::Derivative,
          ExpressionNode::Type::DivisionQuotient,
          ExpressionNode::Type::DivisionRemainder,
          ExpressionNode::Type::Factorial,
          ExpressionNode::Type::Floor,
          ExpressionNode::Type::FracPart,
          ExpressionNode::Type::GreatCommonDivisor,
          ExpressionNode::Type::ImaginaryPart,
          ExpressionNode::Type::Integral,
          ExpressionNode::Type::LeastCommonMultiple,
          ExpressionNode::Type::PermuteCoefficient,
          ExpressionNode::Type::Randint,
          ExpressionNode::Type::RealPart,
          ExpressionNode::Type::Round,
          ExpressionNode::Type::SignFunction,
          ExpressionNode::Type::OUnit,
      })) {
    return !deepIsMatrix(context, canContainMatrices) && !deepIsList(context);
  }

  // These expressions are real if their children are
  if (IsNAry(*this) || isOfType({
                           ExpressionNode::Type::ArcTangent,
                           ExpressionNode::Type::Conjugate,
                           ExpressionNode::Type::Cosine,
                           ExpressionNode::Type::Sine,
                           ExpressionNode::Type::Tangent,
                       })) {
    return allChildrenAreReal(context, canContainMatrices);
  }

  if (otype() == ExpressionNode::Type::ConstantMaths) {
    return static_cast<ConstantNode *>(node())->isReal();
  }

  if (otype() == ExpressionNode::Type::Power) {
    return static_cast<PowerNode *>(node())->isReal(context,
                                                    canContainMatrices);
  }

  // We should return OMG::Troolean::Unknown
  return false;
}

void OExpression::SetReductionEncounteredUndistributedList(bool encounter) {
  s_reductionEncounteredUndistributedList = encounter;
}

/* Comparison */

bool OExpression::isIdenticalTo(const OExpression e) const {
  /* We use the simplification order only because it is a already-coded total
   * order on expresssions. */
#if 0
  return ExpressionNode::SimplificationOrder(node(), e.node(), true) == 0;
#else
  assert(false);
  return false;
#endif
}

bool OExpression::isIdenticalToWithoutParentheses(const OExpression e) const {
  // Same as isIdenticalTo, but ignoring the parentheses.
  return ExpressionNode::SimplificationOrder(node(), e.node(), true, true) == 0;
}

bool OExpression::containsSameDependency(
    const OExpression e, const ReductionContext &reductionContext) const {
  if (isIdenticalToWithoutParentheses(e)) {
    return true;
  }
  if (e.otype() == ExpressionNode::Type::Power &&
      otype() == ExpressionNode::Type::Power &&
      e.childAtIndex(0).isIdenticalToWithoutParentheses(childAtIndex(0))) {
    Power ePower = static_cast<const Power &>(e);
    Power thisPower = static_cast<const Power &>(*this);
    Power::DependencyType depTypeOfE =
        ePower.typeOfDependency(reductionContext);
    Power::DependencyType depTypeOfThis =
        thisPower.typeOfDependency(reductionContext);
    if (depTypeOfThis == depTypeOfE ||
        depTypeOfThis == Power::DependencyType::Both ||
        depTypeOfE == Power::DependencyType::None) {
      return true;
    }
  }
  int n = numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (childAtIndex(i).containsSameDependency(e, reductionContext)) {
      return true;
    }
  }
  return false;
}

/* Layout Helper */

size_t OExpression::serialize(char *buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return isUninitialized()
             ? 0
             : node()->serialize(buffer, bufferSize, floatDisplayMode,
                                 numberOfSignificantDigits);
}

/* Simplification */

OExpression OExpression::cloneAndSimplify(ReductionContext reductionContext,
                                          bool *reductionFailure) const {
  bool reduceFailure = false;
  OExpression e =
      cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
  if (reductionFailure) {
    *reductionFailure = reduceFailure;
  }
  if (reduceFailure ||
      (otype() == ExpressionNode::Type::Store &&
       !static_cast<const OStore *>(this)->isTrulyReducedInShallowReduce())) {
    // We can't beautify unreduced expression
    return e;
  }
  return e.deepBeautify(reductionContext);
}

void makePositive(OExpression *e, bool *isNegative) {
  if (e->otype() == ExpressionNode::Type::Opposite) {
    *isNegative = true;
    *e = e->childAtIndex(0);
  }
}

void OExpression::beautifyAndApproximateScalar(
    OExpression *simplifiedExpression, OExpression *approximateExpression,
    ReductionContext userReductionContext) {
  bool hasUnits = hasUnit();
  ApproximationContext approximationContext(userReductionContext);
  Context *context = userReductionContext.context();
  Preferences::ComplexFormat complexFormat =
      userReductionContext.complexFormat();
  /* Case 1: the reduced expression is ComplexCartesian or pure real, we can
   * take into account the complex format to display a+i*b or r*e^(i*th) */
  if ((otype() == ExpressionNode::Type::ComplexCartesian || isReal(context)) &&
      !hasUnits && !s_reductionEncounteredUndistributedList) {
    ComplexCartesian ecomplex =
        otype() == ExpressionNode::Type::ComplexCartesian
            ? convert<ComplexCartesian>()
            : ComplexCartesian::Builder(*this, Rational::Builder(0));
    if (approximateExpression) {
      /* Step 1: Approximation
       * We compute the approximate expression from the Cartesian form to avoid
       * unprecision. For example, if the result is the ComplexCartesian(a,b),
       * the final expression is going to be sqrt(a^2+b^2)*exp(i*atan(b/a)...
       * in Polar ComplexFormat. If we approximate this expression instead of
       * ComplexCartesian(a,b), we are going to loose precision on the resulting
       * complex.*/
      // Clone the ComplexCartesian to use it to compute the approximation
      ComplexCartesian ecomplexClone =
          ecomplex.clone().convert<ComplexCartesian>();
      /* To minimize the error on the approximation, we reduce the number of
       * nodes in the expression by beautifying */
      ecomplexClone.real().deepBeautify(userReductionContext);
      ecomplexClone.imag().deepBeautify(userReductionContext);
      *approximateExpression =
          ecomplexClone.approximate<double>(approximationContext);
    }
    // Step 2: create the simplified expression with the required complex format
    OExpression ra = complexFormat == Preferences::ComplexFormat::Polar
                         ? ecomplex.clone()
                               .convert<ComplexCartesian>()
                               .norm(userReductionContext)
                               .shallowReduce(userReductionContext)
                         : ecomplex.real();
    OExpression tb = complexFormat == Preferences::ComplexFormat::Polar
                         ? ecomplex.argument(userReductionContext)
                               .shallowReduce(userReductionContext)
                         : ecomplex.imag();
    ra = ra.deepBeautify(userReductionContext);
    tb = tb.deepBeautify(userReductionContext);
    bool raIsNegative = false;
    bool tbIsNegative = false;
    makePositive(&ra, &raIsNegative);
    makePositive(&tb, &tbIsNegative);
    *simplifiedExpression = CreateComplexExpression(ra, tb, complexFormat,
                                                    raIsNegative, tbIsNegative);
  } else {
    // Step 1: beautifying
    *simplifiedExpression = deepBeautify(userReductionContext);
    // Step 2: approximation
    if (approximateExpression) {
      *approximateExpression =
          simplifiedExpression->approximateKeepingUnits<double>(
              userReductionContext);
    }
  }
}

void OExpression::SimplifyAndApproximateChildren(
    OExpression input, OExpression *simplifiedOutput,
    OExpression *approximateOutput, const ReductionContext &reductionContext) {
  assert(input.isOfType(
      {ExpressionNode::Type::OMatrix, ExpressionNode::Type::OList}));
  OList simplifiedChildren = OList::Builder(),
        approximatedChildren = OList::Builder();
  int n = input.numberOfChildren();
  for (int i = 0; i < n; i++) {
    OExpression simplifiedChild, approximateChild;
    OExpression *approximateChildAddress =
        approximateOutput ? &approximateChild : nullptr;
    OExpression childI = input.childAtIndex(i);
    childI.beautifyAndApproximateScalar(
        &simplifiedChild, approximateChildAddress, reductionContext);
    simplifiedChildren.addChildAtIndexInPlace(simplifiedChild, i, i);
    if (approximateOutput) {
      assert(!approximateChild.isUninitialized());
      /* Clone the child in case it was set to the same node as simplified
       * child. This can happen when beautifying an unreduced matrix. */
      approximatedChildren.addChildAtIndexInPlace(approximateChild.clone(), i,
                                                  i);
    }
  }

  if (input.otype() == ExpressionNode::Type::OList) {
    *simplifiedOutput = simplifiedChildren;
    if (approximateOutput) {
      *approximateOutput = approximatedChildren;
    }
    return;
  }

  assert(input.otype() == ExpressionNode::Type::OMatrix);
  OMatrix simplifiedMatrix = OMatrix::Builder(),
          approximateMatrix = OMatrix::Builder();
  for (int i = 0; i < n; i++) {
    simplifiedMatrix.addChildAtIndexInPlace(simplifiedChildren.childAtIndex(i),
                                            i, i);
    if (approximateOutput) {
      approximateMatrix.addChildAtIndexInPlace(
          approximatedChildren.childAtIndex(i), i, i);
    }
  }
  OMatrix m = static_cast<OMatrix &>(input);
  simplifiedMatrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  *simplifiedOutput = simplifiedMatrix;
  if (approximateOutput) {
    approximateMatrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
    *approximateOutput = approximateMatrix;
  }
}

void OExpression::cloneAndSimplifyAndApproximate(
    OExpression *simplifiedExpression, OExpression *approximateExpression,
    const ReductionContext &reductionContext,
    bool approximateKeepingSymbols) const {
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  assert(!approximateExpression || approximateExpression->isUninitialized());

  s_reductionEncounteredUndistributedList = false;

  // Step 1: we reduce the expression
  assert(reductionContext.target() == ReductionTarget::User);
  ReductionContext reductionContextClone = reductionContext;
  bool reduceFailure = false;
  OExpression e = cloneAndDeepReduceWithSystemCheckpoint(
      &reductionContextClone, &reduceFailure, approximateKeepingSymbols);

  if (reduceFailure ||
      (otype() == ExpressionNode::Type::Store &&
       !static_cast<const OStore *>(this)->isTrulyReducedInShallowReduce())) {
    // We can't beautify unreduced expression
    *simplifiedExpression = e;
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximate<double>(
          ApproximationContext(reductionContext));
    }
    return;
  }

  // Step 2: we approximate and beautify the reduced expression
  /* Case 1: the reduced expression is a matrix or a list : We scan the
   * children to beautify them with the right complex format. */
  if (e.isOfType(
          {ExpressionNode::Type::OMatrix, ExpressionNode::Type::OList})) {
    SimplifyAndApproximateChildren(e, simplifiedExpression,
                                   approximateExpression, reductionContext);
  } else {
    /* Case 2: the reduced expression is scalar or too complex to respect the
     * complex format. */
    e.beautifyAndApproximateScalar(simplifiedExpression, approximateExpression,
                                   reductionContext);
  }
}

OExpression OExpression::radianToAngleUnit(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*180/Pi
    return Multiplication::Builder(
        *this, Rational::Builder(180),
        Power::Builder(Constant::PiBuilder(), Rational::Builder(-1)));
  } else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*200/Pi
    return Multiplication::Builder(
        *this, Rational::Builder(200),
        Power::Builder(Constant::PiBuilder(), Rational::Builder(-1)));
  }
  assert(angleUnit == Preferences::AngleUnit::Radian);
  return *this;
}

OExpression OExpression::angleUnitToRadian(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*Pi/180
    return Multiplication::Builder(*this, Rational::Builder(1, 180),
                                   Constant::PiBuilder());
  } else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*Pi/200
    return Multiplication::Builder(*this, Rational::Builder(1, 200),
                                   Constant::PiBuilder());
  }
  assert(angleUnit == Preferences::AngleUnit::Radian);
  return *this;
}

OExpression OExpression::cloneAndReduceAndRemoveUnit(
    ReductionContext reductionContext, OExpression *unit) const {
  bool reductionFailed = false;
  OExpression e = cloneAndDeepReduceWithSystemCheckpoint(&reductionContext,
                                                         &reductionFailed);
  return reductionFailed ? e : e.removeUnit(unit);
}

OExpression OExpression::cloneAndDeepReduceWithSystemCheckpoint(
    ReductionContext *reductionContext, bool *reduceFailure,
    bool approximateDuringReduction) const {
  /* We tried first with the supplied ReductionTarget. If the reduction failed
   * without any user interruption (too many nodes were generated), we try again
   * with ReductionTarget::SystemForApproximation. */
  *reduceFailure = false;
  OExpression e;
  {
    PoolObject *treePoolCursor = Pool::sharedPool->cursor();
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      OExpression reduced = clone().deepReduce(*reductionContext);
      if (approximateDuringReduction) {
        /* It is always needed to reduce when approximating keeping symbols to
         * catch reduction failure and abort if necessary.
         *
         * The expression is reduced before and not during approximation keeping
         * symbols even because deepApproximateKeepingSymbols can only partially
         * reduce the expression.
         *
         * For example, if e="x*x+x^2":
         * "x*x" will be reduced to "x^rational(2)", while "x^2" will be
         * reduced/approximated to "x^float(2.)".
         * Then "x^rational(2)+x^float(2.)" won't be able to reduce to
         * "2*x^float(2.)" because float(2.) != rational(2.).
         * This does not happen if e is reduced beforehand. */
        reduced = reduced.deepApproximateKeepingSymbols(*reductionContext);
      }
      e = reduced;
    } else {
      /* We don't want to tidy all the Pool in the case we are in a nested
       * cloneAndDeepReduceWithSystemCheckpoint: cleaning all the pool might
       * discard ExpressionHandles that are used by parent
       * cloneAndDeepReduceWithSystemCheckpoint. */
      reductionContext->context()->tidyDownstreamPoolFrom(treePoolCursor);
      if (reductionContext->target() !=
          ReductionTarget::SystemForApproximation) {
        // System interruption, try again with another ReductionTarget
        reductionContext->setTarget(ReductionTarget::SystemForApproximation);
        e = clone().deepReduce(*reductionContext);
        if (approximateDuringReduction) {
          e = e.deepApproximateKeepingSymbols(*reductionContext);
        }
      } else {
        *reduceFailure = true;
      }
    }
  }
  if (*reduceFailure) {
    // Cloning outside of ecp's scope in case it raises an exception
    e = clone();
    // Replace symbols
    e = e.deepReplaceSymbols(*reductionContext);
    // Check undef
    if (e.recursivelyMatches(OExpression::IsUndefined,
                             reductionContext->context(),
                             SymbolicComputation::KeepAllSymbols)) {
      return Undefined::Builder();
    }
  } else {
    e = e.deepRemoveUselessDependencies(*reductionContext);
  }
  assert(!e.isUninitialized());
  return e;
}

OExpression OExpression::cloneAndReduce(
    ReductionContext reductionContext) const {
  // TODO: Ensure all cloneAndReduce usages handle reduction failure.
  bool reduceFailure;
  return cloneAndDeepReduceWithSystemCheckpoint(&reductionContext,
                                                &reduceFailure);
}

OExpression OExpression::deepReduce(ReductionContext reductionContext) {
  /* WARNING: This condition is to prevent logarithm of being expanded and
   * create more complex expressions that either could not be integrated
   * because it create terms of sums that are too big, or generate
   * exponentially complex derivative when derivating multiple time.
   * This can be set for Derivative and Integral because the exact result
   * of these expression is never displayed to the user.
   * Examples where it's useful:
   *  - d^4/dx^4(log(x), x, x)
   *  - int(ln(1.0025)*e^(x*ln(1.0025)+ln(200), x, 0, 1000)
   * TODO: This solution is obviously not ideal and the simplification
   * of logarithm should be reworked. */
  if (isOfType(
          {ExpressionNode::Type::Derivative, ExpressionNode::Type::Integral})) {
    reductionContext.setExpandLogarithm(false);
  }
  deepReduceChildren(reductionContext);
  return shallowReduce(reductionContext);
}

OExpression OExpression::deepRemoveUselessDependencies(
    const ReductionContext &reductionContext) {
  OExpression result = *this;
  if (otype() == ExpressionNode::Type::Dependency) {
    Dependency depThis = static_cast<Dependency &>(*this);
    result = depThis.removeUselessDependencies(reductionContext);
  }
  for (int i = 0; i < result.numberOfChildren(); i++) {
    result.childAtIndex(i).deepRemoveUselessDependencies(reductionContext);
  }
  return result;
}

OExpression OExpression::deepReplaceSymbols(
    const ReductionContext &reductionContext) {
  return Undefined::Builder();
}

OExpression OExpression::setSign(bool positive,
                                 const ReductionContext &reductionContext) {
  if (isNumber()) {
    // Needed to avoid infinite loop in Multiplication::shallowReduce
    Number thisNumber = static_cast<Number &>(*this);
    return thisNumber.setSign(positive);
  }
  OMG::Troolean currentSignPositive = isPositive(reductionContext.context());
  assert(currentSignPositive != OMG::Troolean::Unknown);
  if (OMG::BoolToTroolean(positive) == currentSignPositive) {
    return *this;
  }
  Multiplication revertedSign = Multiplication::Builder(Rational::Builder(-1));
  replaceWithInPlace(revertedSign);
  revertedSign.addChildAtIndexInPlace(*this, 1, 1);
  return revertedSign.shallowReduce(reductionContext);
}

int OExpression::lengthOfListChildren() const {
  int lastLength = k_noList;
  int n = numberOfChildren();
  bool isNAry = IsNAry(*this);
  for (int i = n - 1; i >= 0; i--) {
    if (isNAry && childAtIndex(i).otype() < ExpressionNode::Type::OList) {
      return lastLength;
    }
    if (childAtIndex(i).otype() == ExpressionNode::Type::OList) {
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

template <typename U>
OExpression OExpression::approximateKeepingUnits(
    const ReductionContext &reductionContext) const {
  // OUnit need to be extracted before approximating.
  OExpression units;
  OExpression expressionWithoutUnits = *this;
  if (hasUnit()) {
    ReductionContext childContext = reductionContext;
    childContext.setUnitConversion(UnitConversion::None);
    expressionWithoutUnits = cloneAndReduceAndRemoveUnit(childContext, &units);
  }
  OExpression approximationWithoutUnits = expressionWithoutUnits.approximate<U>(
      ApproximationContext(reductionContext));
  if (units.isUninitialized()) {
    return approximationWithoutUnits;
  }
  return Multiplication::Builder(approximationWithoutUnits, units);
}

OExpression OExpression::deepApproximateKeepingSymbols(
    ReductionContext reductionContext, bool *parentCanApproximate,
    bool *parentShouldReduce) {
  *parentCanApproximate = false;
  *parentShouldReduce = false;
  bool thisCanApproximate, thisShouldReduce;
  deepApproximateChildrenKeepingSymbols(reductionContext, &thisCanApproximate,
                                        &thisShouldReduce);
  if (thisCanApproximate) {
    if (otype() == ExpressionNode::Type::Rational) {
      /* It's better not to approximate rational because some reduction and
       * approximation routines check for the presence of rationals to compute
       * properly (like for example PowerNode::templatedApproximate).
       * Currently, approximateKeepingSymbols is used:
       *  - For the exact solver solutions when exact results are forbidden by
       *    the exam mode. In this case, we can keep rationals because "Forbid
       *    exact results" allows to display fractions.
       *  - For the expressionApproximated of ContinuousFunctions which use this
       *    to have an expression faster to evaluate when graphing. In this
       *    case, we NEED to keep rationals, so that
       *    PowerNode::templateApproximated works correctly. (if not, x^(1/3)
       *    would be undef for x < 0. in RealMode).
       */
      *parentCanApproximate = true;
    } else if (otype() != ExpressionNode::Type::Symbol &&
               otype() != ExpressionNode::Type::OList &&
               otype() != ExpressionNode::Type::OMatrix && !isRandom()) {
      /* No need to approximate lists and matrices. Approximating their children
       * is enough.
       * Do not approximate symbols because we are "KeepingSymbols".
       * Do not approximate random because it can be considered as a
       * symbol that always changes values each time it's evaluated.
       * */
      OExpression a =
          approximate<double>(ApproximationContext(reductionContext));
      replaceWithInPlace(a);
      *parentCanApproximate = true;
      *parentShouldReduce = true;
      /* approximate can return an Opposite or a Subtraction, so we need to
       * re-reduce the expression.*/
      return a.shallowReduce(reductionContext);
    }
  }

  if (thisShouldReduce) {
    /* If at least 1 child was approximated, re-reduce.
     * Example: if this is "x + cos(3) + cos(2)",
     * after approximating children it becomes "x - 0.99 - 0.41".
     * It needs now to be reduced to "x - 1.4" */
    *parentShouldReduce = true;
    return shallowReduce(reductionContext);
  }

  return *this;
}

void OExpression::deepApproximateChildrenKeepingSymbols(
    const ReductionContext &reductionContext, bool *canApproximate,
    bool *shouldReduce) {
  *canApproximate = true;
  *shouldReduce = false;
  const int childrenCount = numberOfChildren();
  bool parameteredExpression = isParameteredExpression();
  bool storeExpression = otype() == ExpressionNode::Type::Store;

  for (int i = 0; i < childrenCount; i++) {
    OExpression child = childAtIndex(i);
    /* Do not approximate:
     * - the parameter of parametered expression.
     * - right of a store.
     * - if child is e and it's the base of a log (so that `log(...,e)` can
     *   be later beautified into `ln(...)`).
     */
    if ((parameteredExpression &&
         i == ParameteredExpression::ParameterChildIndex()) ||
        (storeExpression && i == 1) ||
        (otype() == ExpressionNode::Type::Logarithm && i == 1 &&
         child.otype() == ExpressionNode::Type::ConstantMaths &&
         static_cast<Constant &>(child).isExponentialE())) {
      continue;
    }

    bool thisCanApproximate, thisShouldReduce;
    childAtIndex(i).deepApproximateKeepingSymbols(
        reductionContext, &thisCanApproximate, &thisShouldReduce);

    if (!thisCanApproximate && parameteredExpression &&
        i == ParameteredExpression::ParameteredChildIndex()) {
      /* When approximating ParameteredChild keeping symbols,
       * thisCanApproximate will yield to false if the child contains the
       * symbol of the parametered expression. But if it contains no other
       * symbols, it can be approximated. So we re-check if it contains
       * symbols with getVariables.
       * Example:
       * - sum(1, k, 0, 10) -> thisCanApproximate == true, no problem
       * - sum(k, k, 0, 10) -> thisCanApproximate == false, but should be
       * true. getVariables() == 0
       * - sum(kx, k, 0, 10) -> thisCanApproximate == false, and should stay
       * false. getVariables() == 1 */
      char variables[Poincare::OExpression::k_maxNumberOfVariables]
                    [SymbolHelper::k_maxNameSize] = {""};
      int nVariables = getVariables(
          reductionContext.context(),
          [](const char *, Context *) { return true; }, variables[0],
          SymbolHelper::k_maxNameSize);
      thisCanApproximate = (nVariables == 0);
    }
    /* If at least 1 child failed approximation, no need to approximate: it
     * means it has symbols */
    *canApproximate = *canApproximate && thisCanApproximate;
    /* If at least 1 child changed, re-reduce its parent. */
    *shouldReduce = *shouldReduce || thisShouldReduce;
  }

  if (storeExpression) {
    *canApproximate = false;
    *shouldReduce = false;
  }
}

/* Builder */
OExpression OExpression::CreateComplexExpression(
    OExpression ra, OExpression tb, Preferences::ComplexFormat complexFormat,
    bool isNegativeRa, bool isNegativeTb) {
  if (ra.isUndefined() || tb.isUndefined()) {
    return Undefined::Builder();
  }
  OList dependencies = OList::Builder();
  if (ra.otype() == ExpressionNode::Type::Dependency) {
    ra = static_cast<Dependency &>(ra).extractDependencies(dependencies);
  }
  if (tb.otype() == ExpressionNode::Type::Dependency) {
    tb = static_cast<Dependency &>(tb).extractDependencies(dependencies);
  }
  bool isZeroRa = ra.isZero();
  bool isOneRa = ra.isOne();
  bool isZeroTb = tb.isZero();
  bool isOneTb = tb.isOne();
  OExpression result;
  switch (complexFormat) {
    case Preferences::ComplexFormat::Real:
    case Preferences::ComplexFormat::Cartesian: {
      OExpression real;
      OExpression imag;
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
          imag = Constant::ComplexIBuilder();
        } else {
          imag = Multiplication::Builder(tb, Constant::ComplexIBuilder());
          imag.shallowAddMissingParenthesis();
        }
      }
      if (imag.isUninitialized()) {
        result = real;
        break;
      } else if (real.isUninitialized()) {
        if (isNegativeTb) {
          result = Opposite::Builder(imag);
        } else {
          result = imag;
          break;
        }
      } else if (isNegativeTb) {
        result = Subtraction::Builder(real, imag);
      } else {
        result = Addition::Builder(real, imag);
      }
      result.shallowAddMissingParenthesis();
      break;
    }
    default: {
      assert(complexFormat == Preferences::ComplexFormat::Polar);
      OExpression norm;
      OExpression exp;
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
        OExpression arg;
        if (isOneTb) {
          arg = Constant::ComplexIBuilder();
        } else {
          arg = Multiplication::Builder(tb, Constant::ComplexIBuilder());
        }
        if (isNegativeTb) {
          arg = Opposite::Builder(arg);
        }
        arg.shallowAddMissingParenthesis();
        exp = Power::Builder(Constant::ExponentialEBuilder(), arg);
        exp.shallowAddMissingParenthesis();
      }
      if (exp.isUninitialized()) {
        result = norm;
      } else if (norm.isUninitialized()) {
        result = exp;
      } else {
        result = Multiplication::Builder(norm, exp);
        result.shallowAddMissingParenthesis();
      }
    }
  }

  if (dependencies.numberOfChildren() > 0) {
    result = Dependency::Builder(result, dependencies);
  }
  return result;
}

static OExpression maker(OExpression children, int nbChildren,
                         PoolObject::Initializer initializer, size_t size) {
  assert(children.otype() == ExpressionNode::Type::OList);
  PoolHandle handle = PoolHandle::Builder(initializer, size, nbChildren);
  OExpression result = static_cast<OExpression &>(handle);
  for (size_t i = 0; i < static_cast<size_t>(nbChildren); i++) {
    result.replaceChildAtIndexInPlace(i, children.childAtIndex(i));
  }
  return result;
}

OExpression OExpression::FunctionHelper::build(OExpression children) const {
  if (m_untypedBuilder) {
    return (*m_untypedBuilder)(children);
  }
  int numberOfChildren = children.numberOfChildren();
  assert(numberOfChildren >= m_minNumberOfChildren &&
         numberOfChildren <= m_maxNumberOfChildren);
  return maker(children, numberOfChildren, m_initializer, m_size);
}

template OExpression OExpression::approximateKeepingUnits<double>(
    const ReductionContext &reductionContext) const;

}  // namespace Poincare
