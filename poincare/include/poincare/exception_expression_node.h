#ifndef POINCARE_EXCEPTION_EXPRESSION_NODE_H
#define POINCARE_EXCEPTION_EXPRESSION_NODE_H

#include <poincare/exception_node.h>
#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/complex.h>
#include <poincare/char_layout_node.h>
#include <stdio.h>

namespace Poincare {

template <typename T>
class ExceptionExpressionNode : public ExceptionNode<T> {
public:
  // ExpressionNode
  void setChildrenInPlace(Expression other) override {}
  ExpressionNode::Sign sign() const override { return ExpressionNode::Sign::Unknown; }
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) override { return Expression(this).clone(); }
  int polynomialDegree(char symbolName) const override { return -1; }

  Evaluation<float> approximate(ExpressionNode::SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined(); }
  Evaluation<double> approximate(ExpressionNode::DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined(); }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    if (bufferSize == 0) {
      return -1;
    }
    return PrintFloat::convertFloatToText<float>(NAN, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
  }

  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return CharLayoutRef('a'); } //TODO ?

  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override { return Expression(this).clone(); }
};

}

#endif
