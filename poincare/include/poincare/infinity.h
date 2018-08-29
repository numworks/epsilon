#ifndef POINCARE_INFINITY_H
#define POINCARE_INFINITY_H

#include <poincare/number.h>

namespace Poincare {

class AllocationFailureInfinityNode;

class InfinityNode : public NumberNode {
public:
  static InfinityNode * FailedAllocationStaticNode();
  InfinityNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  void setNegative(bool negative) { m_negative = negative; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const override;

  // TreeNode
  size_t size() const override { return sizeof(InfinityNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Infinity";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " negative=\"" << m_negative << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Infinity; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> Evaluation<T> templatedApproximate() const;
  bool m_negative;
};

class Infinity : public Number {
public:
  Infinity(bool negative) : Number(TreePool::sharedPool()->createTreeNode<InfinityNode>()) {
    node()->setNegative(negative);
  }
private:
  InfinityNode * node() const override { return static_cast<InfinityNode *>(Number::node()); }
};

}

#endif
