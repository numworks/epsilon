#include <poincare/and.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/based_integer.h>
#include <cmath>

namespace Poincare
{

  constexpr Expression::FunctionHelper And::s_functionHelper;

  int AndNode::numberOfChildren() const { return And::s_functionHelper.numberOfChildren(); }

  Layout AndNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const
  {
    return LayoutHelper::Prefix(And(this), floatDisplayMode, numberOfSignificantDigits, And::s_functionHelper.name());
  }

  int AndNode::serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const
  {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, And::s_functionHelper.name());
  }

  Expression AndNode::shallowReduce(ReductionContext reductionContext)
  {
    return And(this).shallowReduce(reductionContext);
  }

  Expression And::shallowReduce(ExpressionNode::ReductionContext reductionContext)
  {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined())
      {
        return e;
      }
    }
    Expression a = childAtIndex(0);
    Expression b = childAtIndex(1);
    if (a.type() == ExpressionNode::Type::Matrix)
    {
      return mapOnMatrixFirstChild(reductionContext);
    }
    if (a.type() == ExpressionNode::Type::BasedInteger)
    {
      BasedInteger abased = static_cast<BasedInteger &>(a);
    }

    if (a.type() != ExpressionNode::Type::Rational)
    {
      return Undefined::Builder();
    }
    if (b.type() != ExpressionNode::Type::Rational)
    {
      return Undefined::Builder();
    }
    Rational ar = static_cast<Rational &>(a);
    Rational br = static_cast<Rational &>(b);
    if (!ar.integerDenominator().isOne())
    {
      return Undefined::Builder();
    }
    if (!br.integerDenominator().isOne())
    {
      return Undefined::Builder();
    }
    if (ar.signedIntegerNumerator().isNegative())
    {
      return Undefined::Builder();
    }
    if (br.signedIntegerNumerator().isNegative())
    {
      return Undefined::Builder();
    }

    Integer aq = ar.signedIntegerNumerator();
    Integer bq = br.signedIntegerNumerator();
    Integer x = Integer::LogicalAnd(aq, bq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

} // namespace Poincare
