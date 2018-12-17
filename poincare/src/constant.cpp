#include <poincare/constant.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

ExpressionNode::Sign ConstantNode::sign(Context * context, Preferences::AngleUnit angleUnit) const {
  if (isPi() || isExponential()) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

bool ConstantNode::isReal(Context & context, Preferences::AngleUnit angleUnit) const {
  return !isIComplex();
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
Evaluation<T> ConstantNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  if (isIComplex()) {
    assert(m_name[1] == 0);
    return Complex<T>(0.0, 1.0);
  }
  if (isPi()) {
    return Complex<T>(M_PI);
  }
  assert(isExponential());
  return Complex<T>(M_E);
}

Expression ConstantNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Constant(this).shallowReduce(context, angleUnit, target);
}

Constant::Constant(char name) : SymbolAbstract(TreePool::sharedPool()->createTreeNode<ConstantNode>(SymbolAbstract::AlignedNodeSize(1, sizeof(ConstantNode)))) {
  node()->setName(&name, 1);
}

Expression Constant::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  if (target == ExpressionNode::ReductionTarget::User && isIComplex()) {
    ComplexCartesian c = ComplexCartesian::Builder(Rational(0), Rational(1));
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}

}
