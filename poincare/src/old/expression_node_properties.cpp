#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/integral.h>
#include <poincare/old/logarithm.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/percent.h>
#include <poincare/old/power.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/store.h>
#include <poincare/old/symbol.h>
#include <poincare/old/unit_convert.h>

namespace Poincare {

/* Devirtualisation: methods here could be virtual but are overriden only a few
 * times so it's not worth introducing a vtable entry for them */

bool ExpressionNode::isNumber() const {
  return isOfType({Type::BasedInteger, Type::Decimal, Type::Double, Type::Float,
                   Type::Infinity, Type::Nonreal, Type::Rational,
                   Type::Undefined});
}

bool ExpressionNode::isRandomNumber() const {
  return otype() == Type::Random || otype() == Type::Randint;
}

bool ExpressionNode::isRandomList() const {
  return otype() == Type::RandintNoRepeat;
}

bool ExpressionNode::isParameteredExpression() const {
  return isOfType({Type::Derivative, Type::Integral, Type::ListSequence,
                   Type::Sum, Type::Product});
}

bool ExpressionNode::isCombinationOfUnits() const {
  if (otype() == Type::OUnit) {
    return true;
  }
  if (isOfType({Type::Multiplication, Type::Division})) {
    for (ExpressionNode* child : children()) {
      if (!child->isCombinationOfUnits()) {
        return false;
      }
    }
    return true;
  }
  if (otype() == Type::Power) {
    return childAtIndex(0)->isCombinationOfUnits();
  }
  return false;
}

bool ExpressionNode::isSystemSymbol() const {
  return otype() == ExpressionNode::Type::Symbol &&
         OExpression(this).convert<Symbol>().isSystemSymbol();
}

OExpression ExpressionNode::denominator(
    const ReductionContext& reductionContext) const {
  // Coded in pcj in GetNumeratorAndDenominator
  return OExpression();
}

OExpression ExpressionNode::deepBeautify(
    const ReductionContext& reductionContext) {
  if (otype() == Type::UnitConvert) {
    return OExpression(this).convert<UnitConvert>().deepBeautify(
        reductionContext);
  } else if (otype() == Type::PercentAddition) {
    return OExpression(this).convert<PercentAddition>().deepBeautify(
        reductionContext);
  } else {
    OExpression e = shallowBeautify(reductionContext);
    SimplificationHelper::deepBeautifyChildren(e, reductionContext);
    return e;
  }
}

void ExpressionNode::deepReduceChildren(
    const ReductionContext& reductionContext) {
  if (otype() == Type::Store) {
    OExpression(this).convert<OStore>().deepReduceChildren(reductionContext);
    return;
  }
  if (otype() == Type::Logarithm && numberOfChildren() == 2) {
    OExpression(this).convert<Logarithm>().deepReduceChildren(reductionContext);
    return;
  }
  if (otype() == Type::Integral) {
    OExpression(this).convert<Integral>().deepReduceChildren(reductionContext);
    return;
  }
  if (otype() == Type::Derivative) {
    OExpression(this).convert<Derivative>().deepReduceChildren(
        reductionContext);
    return;
  }
  if (otype() == Type::Dependency) {
    OExpression(this).convert<Dependency>().deepReduceChildren(
        reductionContext);
    return;
  }
  if (otype() == Type::UnitConvert) {
    OExpression(this).convert<UnitConvert>().deepReduceChildren(
        reductionContext);
    return;
  }
  SimplificationHelper::defaultDeepReduceChildren(OExpression(this),
                                                  reductionContext);
}

}  // namespace Poincare
