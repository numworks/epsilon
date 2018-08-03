#ifndef POINCARE_INFINITY_H
#define POINCARE_INFINITY_H

#include <poincare/number.h>

namespace Poincare {

class InfinityNode : public NumberNode {
public:
  void setNegative(bool negative) { m_negative = negative; }

  // TreeNode
  size_t size() const override { return sizeof(InfinityNode); }
#if TREE_LOG
  const char * description() const override { return "Infinity";  }
#endif

  // Properties
  Type type() const override { return Type::Infinity; }
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }

  // Approximation
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> EvaluationReference<T> templatedApproximate() const;
  bool m_negative;
};

class InfinityReference : public NumberReference {
public:
  InfinityReference(bool negative) : NumberReference(TreePool::sharedPool()->createTreeNode<InfinityNode>(), true) {
    if (!node->isAllocationFailure()) {
      static_cast<InfinityNode *>(node)->setNegative(negative);
    }
  }
};

}

#endif
