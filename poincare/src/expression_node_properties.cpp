#include <poincare/derivative.h>
#include <poincare/logarithm.h>
#include <poincare/multiplication.h>
#include <poincare/percent.h>
#include <poincare/power.h>
#include <poincare/simplification_helper.h>
#include <poincare/unit_convert.h>

namespace Poincare {

/* Devirtualisation: methods here could be virtual but are overriden only a few
 * times so it's not worth introducing a vtable entry for them */

bool ExpressionNode::isNumber() const { return isOfType({Type::BasedInteger, Type::Decimal, Type::Double, Type::Float, Type::Infinity, Type::Nonreal, Type::Rational, Type::Undefined}); }

bool ExpressionNode::isRandom() const { return isOfType({Type::Random, Type::Randint, Type::RandintNoRepeat}); }

bool ExpressionNode::isParameteredExpression() const { return isOfType({Type::Derivative, Type::Integral, Type::ListSequence, Type::Sum, Type::Product}); }

bool ExpressionNode::isCombinationOfUnits() const {
  if (type() == Type::Unit) {
    return true;
  }
  if (isOfType({Type::Multiplication, Type::Division})) {
    for (ExpressionNode * child : children()) {
      if (!child->isCombinationOfUnits()) {
        return false;
      }
    }
    return true;
  }
  if (type() == Type::Power) {
    return childAtIndex(0)->isCombinationOfUnits();
  }
  return false;
}

Expression ExpressionNode::denominator(const ReductionContext& reductionContext) const {
  if (type() == Type::Multiplication) {
    return Expression(this).convert<Multiplication>().denominator(reductionContext);
  }
  if (type() == Type::Power) {
    return Expression(this).convert<Power>().denominator(reductionContext);
  }
  if (type() == Type::Rational) {
    return Expression(this).convert<Rational>().denominator();
  }
  return Expression();
}

Expression ExpressionNode::deepBeautify(const ExpressionNode::ReductionContext& reductionContext) {
  if (type() == Type::UnitConvert) {
    return Expression(this).convert<UnitConvert>().deepBeautify(reductionContext);
  } else if (type() == Type::PercentAddition) {
    return Expression(this).convert<PercentAddition>().deepBeautify(reductionContext);
  } else {
    Expression e = shallowBeautify(reductionContext);
    SimplificationHelper::deepBeautifyChildren(e, reductionContext);
    return e;
  }
}

void ExpressionNode::deepReduceChildren(const ExpressionNode::ReductionContext& reductionContext) {
  if (type() == Type::Store) {
    return;
  }
  if (type() == Type::Logarithm && numberOfChildren()==2) {
    Expression(this).convert<Logarithm>().deepReduceChildren(reductionContext);
    return;
  }
  if (type() == Type::Derivative) { // Also for integral ?
    Expression(this).convert<Derivative>().deepReduceChildren(reductionContext);
    return;
  }
  if (type() == Type::UnitConvert) {
    Expression(this).convert<UnitConvert>().deepReduceChildren(reductionContext);
    return;
  }
  SimplificationHelper::defaultDeepReduceChildren(Expression(this), reductionContext);
}


}
