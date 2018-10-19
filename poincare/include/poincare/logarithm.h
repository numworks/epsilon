#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/addition.h>
#include <poincare/complex.h>

namespace Poincare {

template<int T>
class LogarithmNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(LogarithmNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Logarithm";
  }
#endif

  // Properties
  Type type() const override { return Type::Logarithm; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
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
  Logarithm(const LogarithmNode<2> * n) : Expression(n) {}
  Logarithm(Expression child1, Expression child2) : Expression(TreePool::sharedPool()->createTreeNode<LogarithmNode<2> >()) {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }
  static const char * Name() { return "log"; }
  static const int NumberOfChildren() { return 2; }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);

private:
  Expression simpleShallowReduce(Context & context, Preferences::AngleUnit angleUnit);
  Integer simplifyLogarithmIntegerBaseInteger(Integer i, Integer & base, Addition & a, bool isDenominator);
  Expression splitLogarithmInteger(Integer i, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit);
  bool parentIsAPowerOfSameBase() const;
};

class CommonLogarithm : public Expression {
public:
  CommonLogarithm(const LogarithmNode<1> * n) : Expression(n) {}
  explicit CommonLogarithm(Expression operand) : Expression(TreePool::sharedPool()->createTreeNode<LogarithmNode<1> >()) {
    replaceChildAtIndexInPlace(0, operand);
  }
  static const char * Name() { return "log"; }
  static const int NumberOfChildren() { return 1; }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
};

}

#endif
