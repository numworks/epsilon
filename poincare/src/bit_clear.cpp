#include <poincare/bit_clear.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/based_integer.h>
#include <cmath>

namespace Poincare
{

  constexpr Expression::FunctionHelper BitClear::s_functionHelper;

  int BitClearNode::numberOfChildren() const { return BitClear::s_functionHelper.numberOfChildren(); }

  Layout BitClearNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const
  {
    return LayoutHelper::Prefix(BitClear(this), floatDisplayMode, numberOfSignificantDigits, BitClear::s_functionHelper.name());
  }

  int BitClearNode::serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const
  {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BitClear::s_functionHelper.name());
  }

  Expression BitClearNode::shallowReduce(ReductionContext reductionContext)
  {
    return BitClear(this).shallowReduce(reductionContext);
  }

  template <typename T>
  Evaluation<T> BitClearNode::templatedApproximate(ApproximationContext approximationContext) const
  {
    return Complex<T>::RealUndefined();
  }

  Expression BitClear::shallowReduce(ExpressionNode::ReductionContext reductionContext)
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
    if (ar.integerDenominator().isNegative())
    {
      return Undefined::Builder();
    }
    if (br.integerDenominator().isNegative())
    {
      return Undefined::Builder();
    }

    Integer aq = ar.signedIntegerNumerator();
    Integer bq = br.signedIntegerNumerator();
    Integer x = Integer::LogicalBitClear(aq, bq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

} // namespace Poincare
