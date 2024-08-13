#include <assert.h>
#include <float.h>
#include <omg/float.h>
#include <omg/ieee754.h>
#include <poincare/layout.h>
#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/point.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>

#include <cmath>

namespace Poincare {

constexpr OExpression::FunctionHelper Derivative::s_functionHelper;

int DerivativeNode::numberOfChildren() const {
  return Derivative::s_functionHelper.numberOfChildren();
}

size_t DerivativeNode::serialize(char* buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  // Case 1: serialize as f'(a)
  if (isValidCondensedForm()) {
    const char* name =
        static_cast<SymbolAbstractNode*>(childAtIndex(0))->name();
    size_t length = strlen(name);
    assert(bufferSize > length);
    strlcpy(buffer, name, bufferSize);
    int order = extractIntegerOrder();
    assert(order > 0);
    if (order <= 2) {
      length += SerializationHelper::CodePoint(
          buffer + length, bufferSize - length, order == 1 ? '\'' : '"');
    } else {
      length += SerializationHelper::CodePoint(buffer + length,
                                               bufferSize - length, '^');
      length += SerializationHelper::CodePoint(buffer + length,
                                               bufferSize - length, '(');
      length += childAtIndex(3)->serialize(buffer + length, bufferSize - length,
                                           floatDisplayMode,
                                           numberOfSignificantDigits);
      length += SerializationHelper::CodePoint(buffer + length,
                                               bufferSize - length, ')');
    }
    length += SerializationHelper::CodePoint(buffer + length,
                                             bufferSize - length, '(');
    length +=
        childAtIndex(2)->serialize(buffer + length, bufferSize - length,
                                   floatDisplayMode, numberOfSignificantDigits);
    length += SerializationHelper::CodePoint(buffer + length,
                                             bufferSize - length, ')');
    return length;
  }
  // Case 2: serialize as diff(f(x),x,a,n)
  int lastChildIndex =
      isFirstOrder() ? numberOfChildren() - 2 : numberOfChildren() - 1;
  Expression e = createValidExpandedForm();
  return SerializationHelper::Prefix(
      e.node(), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Derivative::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::Classic, lastChildIndex);
}

OExpression DerivativeNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Derivative(this).shallowReduce(reductionContext);
}

bool DerivativeNode::isValidCondensedForm() const {
  // Returns true if of the form diff(f(SystemSymbol),SystemSymbol,x,n)
  return childAtIndex(0)->otype() == ExpressionNode::Type::Function &&
         childAtIndex(0)->childAtIndex(0)->isSystemSymbol() &&
         childAtIndex(1)->isSystemSymbol();
}

Expression DerivativeNode::createValidExpandedForm() const {
  return Derivative(this).createValidExpandedForm();
}

int DerivativeNode::extractIntegerOrder() const {
  int derivationOrder;
  bool orderIsSymbol;
  bool orderIsInteger = SimplificationHelper::extractIntegerChildAtIndex(
      Derivative(this), numberOfChildren() - 1, &derivationOrder,
      &orderIsSymbol);
  assert(!orderIsSymbol && orderIsInteger && derivationOrder >= 0);
  (void)orderIsInteger;  // Silence compilation warning.
  return derivationOrder;
}

void Derivative::deepReduceChildren(const ReductionContext& reductionContext) {
  /* First child is reduced with target SystemForAnalysis */
  ReductionContext childContext = reductionContext;
  childContext.setTarget(ReductionTarget::SystemForAnalysis);
  childAtIndex(0).deepReduce(childContext);

  /* Other children are reduced with the same reduction target as the parent */
  const int childrenCount = numberOfChildren();
  assert(childrenCount > 1);
  for (int i = 1; i < childrenCount; i++) {
    childAtIndex(i).deepReduce(reductionContext);
  }
}

OExpression Derivative::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists,
        SimplificationHelper::PointReduction::DefinedOnPoint);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  // Distribute over point
  OExpression point = distributeOverPoint();
  if (!point.isUninitialized()) {
    int n = point.numberOfChildren();
    assert(n == 2);
    for (int i = 0; i < n; i++) {
      point.childAtIndex(i).shallowReduce(reductionContext);
    }
    replaceWithInPlace(point);
    return point.shallowReduce(reductionContext);
  }

  int derivationOrder;
  bool orderIsSymbol;
  bool orderIsInteger = SimplificationHelper::extractIntegerChildAtIndex(
      *this, numberOfChildren() - 1, &derivationOrder, &orderIsSymbol);
  if (orderIsSymbol) {
    return *this;
  }
  if (!orderIsInteger || derivationOrder < 0) {
    return replaceWithUndefinedInPlace();
  }

  OList listOfDependencies = OList::Builder();
  OExpression derivand = childAtIndex(0);
  if (derivand.otype() == ExpressionNode::Type::Dependency) {
    static_cast<Dependency&>(derivand).extractDependencies(listOfDependencies);
    derivand = childAtIndex(0);
  }
  Symbol symbol = childAtIndex(1).convert<Symbol>();
  OExpression symbolValue = childAtIndex(2);
  OExpression derivandAsDependency = derivand.clone();

  /* Use reduction target SystemForAnalysis when derivating,
   * because we don't want to have false reductions such as
   * arccot(x) -> arctan(1/x)
   * This will not impact the function derivate since it only
   * use the angle unit of the reduction context, but it will
   * impact the function deepReduce */
  ReductionTarget initialTarget = reductionContext.target();
  reductionContext.setTarget(ReductionTarget::SystemForAnalysis);

  int currentDerivationOrder = derivationOrder;
  /* Since derivand is a child to the derivative node, it can be replaced in
   * place without derivate having to return the derivative. */
  while (currentDerivationOrder > 0 &&
         derivand.derivate(reductionContext, symbol, symbolValue)) {
    /* We need to reduce the derivand here before replacing the symbol: the
     * general formulas used during the derivation process can create some nodes
     * that are not defined for some values (e.g. log), but that would disappear
     * at reduction. */
    OExpression reducedDerivative =
        childAtIndex(0).deepReduce(reductionContext);
    if (reducedDerivative.otype() == ExpressionNode::Type::Dependency) {
      /* Remove the dependencies created during derivation reduction since they
       * were just generated by the calculation process but do not reflect
       * the real definition domain of the derivative.
       * Example: 3x is derivated to 0*x+3*1 because we use the u'v+uv' formula.
       * So a dependency is created on x (because of 0*x), which makes no
       * sense.*/
      reducedDerivative = reducedDerivative.childAtIndex(0);
    }
    replaceChildAtIndexInPlace(0, reducedDerivative);
    if (reducedDerivative.isZero()) {
      currentDerivationOrder = 0;
    } else {
      currentDerivationOrder--;
    }
    /* Updates the value of derivand, because derivate may call
     * replaceWithInplace on it. */
    derivand = childAtIndex(0);
  }

  if (currentDerivationOrder < derivationOrder) {
    // Do not add a dependecy if nothing was derivated.
    listOfDependencies.addChildAtIndexInPlace(
        derivandAsDependency, listOfDependencies.numberOfChildren(),
        listOfDependencies.numberOfChildren());
  }

  reductionContext.setTarget(initialTarget);

  if (currentDerivationOrder == 0) {
    /* Deep reduces the child, because derivate may not preserve its reduced
     * status. */
    Dependency d = Dependency::Builder(childAtIndex(0), listOfDependencies);
    OExpression result = d.replaceSymbolWithExpression(symbol, symbolValue);
    replaceWithInPlace(result);
    return result.deepReduce(reductionContext);
  }

  /* If derivation fails, we still need to decrease the derivation order
   * according to what was already done. */
  replaceChildAtIndexInPlace(numberOfChildren() - 1,
                             Rational::Builder(currentDerivationOrder));
  Dependency d = Dependency::Builder(Undefined::Builder(), listOfDependencies);
  replaceWithInPlace(d);
  d.replaceChildAtIndexInPlace(0, *this);
  return d.shallowReduce(reductionContext);
}

void Derivative::DerivateUnaryFunction(
    OExpression function, Symbol symbol, OExpression symbolValue,
    const ReductionContext& reductionContext) {
  {
    OExpression e =
        Derivative::DefaultDerivate(function, reductionContext, symbol);
    if (!e.isUninitialized()) {
      return;
    }
  }

  OExpression df = function.unaryFunctionDifferential(reductionContext);
  OExpression g = function.childAtIndex(0);
  OExpression dg = g.derivate(reductionContext, symbol, symbolValue)
                       ? function.childAtIndex(0)
                       : Derivative::Builder(function.childAtIndex(0),
                                             symbol.clone().convert<Symbol>(),
                                             symbolValue.clone());
  function.replaceWithInPlace(Multiplication::Builder(df, dg));
}

OExpression Derivative::DefaultDerivate(
    OExpression function, const ReductionContext& reductionContext,
    Symbol symbol) {
  int polynomialDegree =
      function.polynomialDegree(reductionContext.context(), symbol.name());
  if (polynomialDegree == 0) {
    OExpression result = Rational::Builder(0);
    function.replaceWithInPlace(result);
    return result;
  }
  return OExpression();
}

OExpression Derivative::UntypedBuilder(OExpression children) {
  assert(children.otype() == ExpressionNode::Type::OList);
  if (children.childAtIndex(1).otype() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return OExpression();
  }
  if (children.numberOfChildren() < 4) {
    return Builder(children.childAtIndex(0),
                   children.childAtIndex(1).convert<Symbol>(),
                   children.childAtIndex(2));
  } else {
    return Builder(children.childAtIndex(0),
                   children.childAtIndex(1).convert<Symbol>(),
                   children.childAtIndex(2), children.childAtIndex(3));
  }
}

OExpression Derivative::distributeOverPoint() {
  OExpression pointParent = *this;
  OExpression point = childAtIndex(0);
  bool hasDependency = false;
  if (point.otype() == ExpressionNode::Type::Dependency) {
    pointParent = point;
    point = pointParent.childAtIndex(0);
    hasDependency = true;
  }
  if (point.otype() != ExpressionNode::Type::OPoint) {
    return OExpression();
  }
  pointParent.replaceChildAtIndexWithGhostInPlace(0);
  int n = point.numberOfChildren();
  assert(n == 2);
  for (int i = 0; i < n; i++) {
    OExpression newDerivand = clone();
    OExpression ghostParent =
        hasDependency ? newDerivand.childAtIndex(0) : newDerivand;
    ghostParent.replaceChildAtIndexInPlace(0, point.childAtIndex(i));
    point.replaceChildAtIndexInPlace(i, newDerivand);
  }
  return point;
}

OExpression Derivative::createValidExpandedForm() const {
  if (childAtIndex(1).isSystemSymbol() && !isValidCondensedForm()) {
    /* An originally condensed form diff(f(SystemSymbol),SystemSymbol,x,n) might
     * have become invalid if the function was replaced by its expression and
     * the derivative was not reduced. */
    OExpression e = clone();
    e.childAtIndex(0).replaceSymbolWithExpression(Symbol::SystemSymbol(),
                                                  Symbol::Builder('x'));
    e.childAtIndex(1).replaceSymbolWithExpression(Symbol::SystemSymbol(),
                                                  Symbol::Builder('x'));
    return e;
  }
  return *this;
}

}  // namespace Poincare
