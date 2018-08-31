#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class FactorialNode : public ExpressionNode  {
public:
  // Allocation Failure
  static FactorialNode * FailedAllocationStaticNode();
  FactorialNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(FactorialNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Factorial";
  }
#endif

  // Properties
  Type type() const override { return Type::Factorial; }
private:
  // Layout
  bool needsParenthesesWithParent(const SerializationHelperInterface * parentNode) const override;
  LayoutReference createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) override;
  Expression shallowBeautify(Context& context, Preferences::AngleUnit angleUnit) const override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }

#if 0
  int simplificationOrderGreaterType(const Expression e) const override;
  int simplificationOrderSameType(const Expression e) const override;
#endif
};

class Factorial : public Expression {
public:
  Factorial() : Expression(TreePool::sharedPool()->createTreeNode<FactorialNode>()) {}
  Factorial(const FactorialNode * n) : Expression(n) {}
  Factorial(Expression operand) : Factorial() {
    replaceChildAtIndexInPlace(0, operand);
  }

  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const;
private:
  constexpr static int k_maxOperandValue = 100;
};

}

#endif
