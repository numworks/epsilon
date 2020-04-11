#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/sequence.h>

namespace Poincare {

class SumNode final : public SequenceNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(SumNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Sum";
  }
#endif

  Type type() const override { return Type::Sum; }

private:
  float emptySequenceValue() const override { return 0.0f; }
  Layout createSequenceLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Evaluation<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> a, Evaluation<double> b, Preferences::ComplexFormat complexFormat) const override {
    return templatedApproximateWithNextTerm<double>(a, b, complexFormat);
  }
  Evaluation<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> a, Evaluation<float> b, Preferences::ComplexFormat complexFormat) const override {
    return templatedApproximateWithNextTerm<float>(a, b, complexFormat);
  }
  template<typename T> Evaluation<T> templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b, Preferences::ComplexFormat complexFormat) const;
};

class Sum final : public Sequence {
friend class SumNode;
public:
  Sum(const SumNode * n) : Sequence(n) {}
  static Sum Builder(Expression child0, Symbol child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<Sum, SumNode>({child0, child1, child2, child3}); }
  static Expression UntypedBuilder(Expression children);

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sum", 4, &UntypedBuilder);
};

}

#endif
