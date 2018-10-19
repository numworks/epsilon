#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/sequence.h>

namespace Poincare {

class SumNode final : public SequenceNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(SumNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Sum";
  }
#endif

  Type type() const override { return Type::Sum; }
private:
  const char * name() const override { return "sum"; }
  float emptySequenceValue() const override { return 0.0f; }
  Layout createSequenceLayout(Layout argumentLayout, Layout subscriptLayout, Layout superscriptLayout) const override;
  Evaluation<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> a, Evaluation<double> b) const override {
    return templatedApproximateWithNextTerm<double>(a, b);
  }
  Evaluation<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> a, Evaluation<float> b) const override {
    return templatedApproximateWithNextTerm<float>(a, b);
  }
  template<typename T> Evaluation<T> templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b) const;
};

class Sum final : public Expression {
friend class SumNode;
public:
  Sum();
  Sum(const SumNode * n) : Expression(n) {}
  Sum(Expression operand0, Expression operand1, Expression operand2) : Sum() {
    replaceChildAtIndexInPlace(0, operand0);
    replaceChildAtIndexInPlace(1, operand1);
    replaceChildAtIndexInPlace(2, operand2);
  }
};

}

#endif
