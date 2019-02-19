#include <poincare/constant.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <poincare/unreal.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

ConstantNode::ConstantNode(const char * newName, int length) : SymbolAbstractNode() {
  strlcpy(const_cast<char*>(name()), newName, length+1);
}

ExpressionNode::Sign ConstantNode::sign(Context * context) const {
  if (isPi() || isExponential()) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

bool ConstantNode::isReal(Context & context) const {
  return !isIComplex();
}

int rankOfConstant(char c) {
  switch (c) {
    case Ion::Charset::IComplex:
      return 0;
    case Ion::Charset::SmallPi:
      return 1;
    case Ion::Charset::Exponential:
      return 2;
    default:
      assert(false);
      return -1;
  }
}

int ConstantNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, canBeInterrupted);
  }
  assert(type() == e->type());
  return (rankOfConstant(name()[0]) - rankOfConstant(static_cast<const ConstantNode *>(e)->name()[0]));
}

Layout ConstantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::String(m_name, strlen(m_name));
}

int ConstantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return strlcpy(buffer, m_name, bufferSize);
}

template<typename T>
Evaluation<T> ConstantNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (isIComplex()) {
    assert(m_name[1] == 0);
    return Complex<T>::Builder(0.0, 1.0);
  }
  if (isPi()) {
    return Complex<T>::Builder(M_PI);
  }
  assert(isExponential());
  return Complex<T>::Builder(M_E);
}

Expression ConstantNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Constant(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Expression Constant::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression result;
  if (complexFormat == Preferences::ComplexFormat::Real && isIComplex()) {
    result = Unreal::Builder();
  } else if (target == ExpressionNode::ReductionTarget::User && isIComplex()) {
    result = ComplexCartesian::Builder(Rational::Builder(0), Rational::Builder(1));
  }
  if (!result.isUninitialized()) {
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

template Evaluation<float> ConstantNode::templatedApproximate<float>(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> ConstantNode::templatedApproximate<double>(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
}
