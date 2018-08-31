#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

template<int I>
class LogarithmNode : public ExpressionNode {
  /* TODO friend class NaperianLogarithm; */
public:
  // Allocation Failure
  static LogarithmNode<I> * FailedAllocationStaticNode();
  LogarithmNode<I> * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(LogarithmNode); }
  int numberOfChildren() const override { assert(I == 1 || I == 2); return I; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Logarithm";
  }
#endif

  // Properties
  Type type() const override { return Type::Logarithm; }

  // Layout
  LayoutReference createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "log");
  }
  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) override;
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
    /* log has a branch cut on ]-inf, 0]: it is then multivalued on this cut. We
     * followed the convention chosen by the lib c++ of llvm on ]-inf+0i, 0+0i]
     * (warning: log takes the other side of the cut values on ]-inf-0i, 0-0i]). */
    return Complex<T>(std::log10(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Logarithm : public Expression {
public:
  Logarithm(const LogarithmNode<1> * n) : Expression(n) {}
  Logarithm(const LogarithmNode<2> * n) : Expression(n) {}
  Logarithm(Expression operand) : Expression(TreePool::sharedPool()->createTreeNode<LogarithmNode<1> >()) {
    replaceChildAtIndexInPlace(0, operand);
  }
  Logarithm(Expression child1, Expression child2) : Expression(TreePool::sharedPool()->createTreeNode<LogarithmNode<2> >()) {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);

private:
  Expression simpleShallowReduce(Context & context, Preferences::AngleUnit angleUnit) const;
  Expression splitInteger(Integer i, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit);
  //bool parentIsAPowerOfSameBase() const;
};

}

#endif
