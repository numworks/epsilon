#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

template<int T>
class LogarithmNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(LogarithmNode); }
  int numberOfChildren() const override { assert(T == 1 || T == 2); return T; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Logarithm";
  }
#endif

  // Properties
  Type type() const override { return Type::Logarithm; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "log");
  }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  template<typename U> static Complex<U> computeOnComplex(const std::complex<U> c, Preferences::AngleUnit angleUnit) {
    /* log has a branch cut on ]-inf, 0]: it is then multivalued on this cut. We
     * followed the convention chosen by the lib c++ of llvm on ]-inf+0i, 0+0i]
     * (warning: log takes the other side of the cut values on ]-inf-0i, 0-0i]). */
    return Complex<U>(std::log10(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename U> Evaluation<U> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Logarithm final : public Expression {
public:
  Logarithm(const LogarithmNode<1> * n) : Expression(n) {}
  Logarithm(const LogarithmNode<2> * n) : Expression(n) {}
  explicit Logarithm(Expression operand) : Expression(TreePool::sharedPool()->createTreeNode<LogarithmNode<1> >()) {
    replaceChildAtIndexInPlace(0, operand);
  }
  Logarithm(Expression child1, Expression child2) : Expression(TreePool::sharedPool()->createTreeNode<LogarithmNode<2> >()) {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);

private:
  Expression simpleShallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Integer simplifyLogarithmIntegerBaseInteger(Integer i, Integer & base, Addition & a, bool isDenominator);
  Expression splitLogarithmInteger(Integer i, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit);
  bool parentIsAPowerOfSameBase() const;
};

}

#endif
