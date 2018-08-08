#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include <poincare/number.h>
#include <poincare/allocation_failure_expression_node.h>

namespace Poincare {

class UndefinedNode : public NumberNode {
public:
  static UndefinedNode * FailedAllocationStaticNode();

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
#if TREE_LOG
  const char * description() const override { return "Undefined";  }
#endif

  // Properties
  Type type() const override { return Type::Undefined; }
  int polynomialDegree(char symbolName) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> Evaluation<T> templatedApproximate() const;
};

class AllocationFailureUndefinedNode : public AllocationFailureExpressionNode<UndefinedNode> {
};

class Undefined : public Number {
public:
  Undefined() : Number(TreePool::sharedPool()->createTreeNode<UndefinedNode>()) {}
};

}

#endif
