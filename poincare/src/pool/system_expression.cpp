#include <poincare/function_properties/integral.h>
#include <poincare/helpers/decimal.h>
#include <poincare/prepared_function.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/system_expression.h>
#include <poincare/user_expression.h>

namespace Poincare {

using namespace Internal;

SystemExpression SystemExpression::ExpressionFromAddress(const void* address,
                                                         size_t size) {
  Expression e = Expression::ExpressionFromAddress(address, size);
  return static_cast<SystemExpression&>(e);
}

SystemExpression SystemExpression::CreateReduce(const Tree* structure,
                                                ContextTrees ctx) {
  Tree* tree = PatternMatching::CreateReduce(structure, ctx);
  return SystemExpression::Builder(tree);
}

SystemExpression SystemExpression::CreateIntegralOfAbsOfDifference(
    SystemExpression lowerBound, SystemExpression upperBound,
    SystemExpression integrandA, SystemExpression integrandB) {
  Tree* integrand = PatternMatching::CreateReduce(
      KAbs(KAdd(KA, KMult(-1_e, KB))), {.KA = integrandA, .KB = integrandB});
  // KUnknownSymbol is the variable used in the grapher app
  return IntegralBetweenBounds(SystemExpression::Builder(integrand),
                               Internal::Symbol::GetName(KUnknownSymbol),
                               lowerBound, upperBound);
}

// Builders from value.
SystemExpression SystemExpression::Builder(int32_t n) {
  return Builder(Integer::Push(n));
}

template <typename T>
SystemExpression SystemExpression::Builder(T x) {
  return Builder(SharedTreeStack->pushFloat(x));
}

template <typename T>
SystemExpression SystemExpression::Builder(Coordinate2D<T> point) {
  return SystemExpression::CreateReduce(
      KPoint(KA, KB),
      {.KA = Builder<T>(point.x()), .KB = Builder<T>(point.y())});
}

template <typename T>
SystemExpression SystemExpression::Builder(
    PointOrRealScalar<T> pointOrRealScalar) {
  if (pointOrRealScalar.isRealScalar()) {
    return Builder<T>(pointOrRealScalar.toRealScalar());
  }
  return Builder<T>(pointOrRealScalar.toPoint());
}

SystemExpression SystemExpression::DecimalBuilderFromDouble(double value) {
  return SystemExpression::Builder(Poincare::DecimalBuilderFromDouble(value));
}

SystemExpression SystemExpression::RationalBuilder(int32_t numerator,
                                                   int32_t denominator) {
  return Builder(
      Rational::Push(IntegerHandler(numerator), IntegerHandler(denominator)));
}
SystemExpression SystemExpression::Undefined() {
  return SystemExpression::Builder(KUndef);
}

SystemExpression SystemExpression::cloneChildAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

template <typename T>
SystemExpression SystemExpression::approximateSystemToTree() const {
  return SystemExpression::Builder(Approximation::ToTree<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                        .prepare = true}));
}

void SystemExpression::cloneAndBeautifyAndApproximate(
    UserExpression* beautifiedExpression,
    UserExpression* approximatedExpression, ProjectionContext& context) const {
  assert(beautifiedExpression && beautifiedExpression->isUninitialized());
  *beautifiedExpression = cloneAndBeautify(context);
  assert(approximatedExpression && approximatedExpression->isUninitialized());
  *approximatedExpression =
      beautifiedExpression->cloneAndApproximate<double>(context);
}

UserExpression SystemExpression::cloneAndBeautify(
    ProjectionContext& context) const {
  Tree* e = tree()->cloneTree();
  Simplification::BeautifyReduced(e, &context, Internal::Dimension::Get(e));
  return UserExpression::Builder(e);
}

/* If reductionFailure is true, skip simplification. TODO: Like similar methods,
 * returned expression is not actually SystemExpression if reduction failed. */
SystemExpression ReplaceSymbolAndReduce(
    const SystemExpression& e, const char* symbolName,
    const SystemExpression& replaceSymbolWith, bool* reductionFailure,
    SymbolicComputation symbolic, bool reduce) {
  ExceptionTry {
    Tree* symbol = SharedTreeStack->pushUserSymbol(symbolName);
    assert(Internal::Dimension::Get(replaceSymbolWith) ==
           Internal::Dimension::Get(symbol));
    TreeRef result = e.tree()->cloneTree();
    result->deepReplaceWith(symbol, replaceSymbolWith);
    symbol->removeTree();
    if (Variables::HasUserSymbols(result, true)) {
      // TODO: Only this case is used and handled for now.
      assert(symbolic == SymbolicComputation::ReplaceAllSymbolsWithUndefined);
      result->cloneTreeOverTree(KUndef);
    }
    if (reduce) {
      /* Note: Advanced reduction could be allowed for slower but better
       * reduction. */
      Simplification::ReduceSystem(result, false);
    }
    return SystemExpression::Builder(static_cast<Tree*>(result));
  }
  ExceptionCatch(exc) {
    switch (exc) {
      case ExceptionType::TreeStackOverflow:
      case ExceptionType::IntegerOverflow:
        if (reduce) {
          *reductionFailure = true;
          // Try again without simplification
          return ReplaceSymbolAndReduce(e, symbolName, replaceSymbolWith,
                                        reductionFailure, symbolic, false);
        }
        [[fallthrough]];
      default:
        TreeStackCheckpoint::Raise(exc);
    }
  }
  OMG_UNREACHABLE;
}

/* TODO: Like similar methods, returned expression is not actually
 * SystemExpression if reduction failed. */
SystemExpression SystemExpression::cloneAndReplaceSymbolWithExpression(
    const char* symbolName, const SystemExpression& replaceSymbolWith,
    bool* reductionFailure, SymbolicComputation symbolic) const {
  assert(reductionFailure);
  *reductionFailure = false;
  return ReplaceSymbolAndReduce(*this, symbolName, replaceSymbolWith,
                                reductionFailure, symbolic, true);
}

SystemExpression SystemExpression::getReducedDerivative(
    const char* symbolName, int derivationOrder) const {
  Tree* result = SharedTreeStack->pushDiff();
  // Symbol (so that local variable is layouted to symbolName)
  const Tree* symbol = SharedTreeStack->pushUserSymbol(symbolName);
  // Symbol value (so that the derivative is evaluated at symbolName's value)
  SharedTreeStack->pushUserSymbol(symbolName);
  Integer::Push(derivationOrder);
  Tree* derivand = tree()->cloneTree();
  Variables::ReplaceSymbol(derivand, symbol, 0,
                           Parametric::VariableProperties(result));
  // Check dimension again as the diff may have changed it.
  if (!Internal::Dimension::DeepCheck(result)) {
    result->cloneTreeOverTree(KUndefUnhandledDimension);
  } else {
    Simplification::ReduceSystem(result, false,
                                 ReductionTarget::SystemForApproximation);
  }
  return SystemExpression::Builder(result);
}

PreparedFunction SystemExpression::getPreparedFunction(const char* symbolName,
                                                       bool scalarsOnly) const {
  Internal::Dimension dimension = Internal::Dimension::Get(tree());
  if ((scalarsOnly &&
       (!dimension.isScalar() || Internal::Dimension::IsList(tree()))) ||
      (!dimension.isScalar() && !dimension.isPoint())) {
    return PreparedFunction::Builder(KUndef);
  }
  Tree* result = tree()->cloneTree();
  Approximation::PrepareFunctionForApproximation(result, symbolName, true);
  return PreparedFunction::Builder(result);
}

template <typename T>
T SystemExpression::approximateSystemToRealScalar() const {
  if (!Internal::Dimension::IsNonListScalar(tree())) {
    return NAN;
  }
  return Approximation::To<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                        .prepare = true});
}

template <typename T>
Coordinate2D<T> SystemExpression::approximateToPoint() const {
  return Approximation::ToPoint<T>(
      tree(), Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                        .prepare = true});
}

template <typename T>
SystemExpression SystemExpression::approximateListAndSort() const {
  assert(dimension().isList());
  Tree* clone = SharedTreeStack->pushListSort();
  tree()->cloneTree();
  clone->moveTreeOverTree(Approximation::ToTree<T>(
      clone, Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                       .prepare = true}));
  return SystemExpression::Builder(clone);
}

SystemExpression SystemExpression::removeUndefAndComplexListElements() const {
  Tree* clone = tree()->cloneTree();
  assert(clone->isList());
  int n = clone->numberOfChildren();
  int remainingChildren = n;
  Tree* child = clone->nextNode();
  for (int i = 0; i < n; i++) {
    if (child->isUndefined() ||
        (Internal::Dimension::Get(child).isScalar() &&
         GetComplexProperties(child).isNonReal()) ||
        (child->isPoint() && child->hasChildSatisfying([](const Tree* e) {
          return e->isUndefined() || GetComplexProperties(e).isNonReal();
        }))) {
      child->removeTree();
      remainingChildren--;
      NAry::SetNumberOfChildren(clone, remainingChildren);
    } else {
      child = child->nextTree();
    }
  }
  return SystemExpression::Builder(clone);
}

int SystemExpression::polynomialDegree(const char* symbolName) const {
  return Degree::Get(tree(), symbolName);
}

int SystemExpression::getPolynomialReducedCoefficients(
    const char* symbolName, SystemExpression coefficients[],
    bool keepDependencies) const {
  Tree* coefList = PolynomialParser::GetReducedCoefficients(tree(), symbolName,
                                                            keepDependencies);
  if (!coefList) {
    return -1;
  }
  int degree = coefList->numberOfChildren() - 1;
  assert(degree >= 0);
  Tree* child = coefList->nextNode();
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = Builder(child);
  }
  coefList->removeNode();
  return degree;
}

Sign SystemExpression::sign() const { return GetSign(tree()); }

/* TODO: Find a better way to avoid duplication of Builder, clone, and
 *       cloneChildAtIndex methods with other types of expressions. */
SystemExpression SystemExpression::Builder(const Tree* tree) {
  PoolHandle p = BuildPoolHandleFromTree(tree);
  return static_cast<SystemExpression&>(p);
}

SystemExpression SystemExpression::Builder(Tree* tree) {
  SystemExpression result = Builder(const_cast<const Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

template SystemExpression SystemExpression::Builder<float>(float);
template SystemExpression SystemExpression::Builder<double>(double);
template SystemExpression SystemExpression::Builder<float>(Coordinate2D<float>);
template SystemExpression SystemExpression::Builder<double>(
    Coordinate2D<double>);
template SystemExpression SystemExpression::Builder<float>(
    PointOrRealScalar<float>);
template SystemExpression SystemExpression::Builder<double>(
    PointOrRealScalar<double>);
template SystemExpression SystemExpression::approximateSystemToTree<float>()
    const;
template SystemExpression SystemExpression::approximateSystemToTree<double>()
    const;
template float SystemExpression::approximateSystemToRealScalar<float>() const;
template double SystemExpression::approximateSystemToRealScalar<double>() const;
template Coordinate2D<float> SystemExpression::approximateToPoint<float>()
    const;
template Coordinate2D<double> SystemExpression::approximateToPoint<double>()
    const;
template SystemExpression SystemExpression::approximateListAndSort<float>()
    const;
template SystemExpression SystemExpression::approximateListAndSort<double>()
    const;

}  // namespace Poincare
