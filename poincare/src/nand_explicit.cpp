#include <poincare/nand_explicit.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/based_integer.h>
#include <cmath>

namespace Poincare
{

  constexpr Expression::FunctionHelper NandExplicit::s_functionHelper;

  int NandExplicitNode::numberOfChildren() const { return NandExplicit::s_functionHelper.numberOfChildren(); }

  Layout NandExplicitNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const
  {
    return LayoutHelper::Prefix(NandExplicit(this), floatDisplayMode, numberOfSignificantDigits, NandExplicit::s_functionHelper.name());
  }

  int NandExplicitNode::serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const
  {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NandExplicit::s_functionHelper.name());
  }

  Expression NandExplicitNode::shallowReduce(ReductionContext reductionContext)
  {
    return NandExplicit(this).shallowReduce(reductionContext);
  }

  template <typename T>
  Evaluation<T> NandExplicitNode::templatedApproximate(ApproximationContext approximationContext) const
  {
    return Complex<T>::RealUndefined();
  }

  Expression NandExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext)
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
    Expression c = childAtIndex(2);

    if (a.type() == ExpressionNode::Type::Matrix)
    {
      return mapOnMatrixFirstChild(reductionContext);
    }

    if (a.type() != ExpressionNode::Type::Rational)
    {
      return Undefined::Builder();
    }
    if (b.type() != ExpressionNode::Type::Rational)
    {
      return Undefined::Builder();
    }
    if (c.type() != ExpressionNode::Type::Rational)
    {
      return Undefined::Builder();
    }
    Rational ar = static_cast<Rational &>(a);
    Rational br = static_cast<Rational &>(b);
    Rational cr = static_cast<Rational &>(c);
    if (!ar.integerDenominator().isOne())
    {
      return Undefined::Builder();
    }
    if (!br.integerDenominator().isOne())
    {
      return Undefined::Builder();
    }
    if (!cr.integerDenominator().isOne())
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
    if (cr.signedIntegerNumerator().isNegative())
    {
      return Undefined::Builder();
    }

    Integer aq = ar.signedIntegerNumerator();
    Integer bq = br.signedIntegerNumerator();
    Integer cq = cr.signedIntegerNumerator();
    Integer x = Integer::LogicalNand(aq, bq, cq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

} // namespace Poincare
