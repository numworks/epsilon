#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include <poincare/number.h>

namespace Poincare {

class UndefinedNode : public NumberNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
#if TREE_LOG
  const char * description() const override { return "Undefined";  }
#endif

  // Properties
  Type type() const override { return Type::Undefined; }
  int polynomialDegree(char symbolName) const override;

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
};

class UndefinedReference : public NumberReference {
public:
  UndefinedReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<UndefinedNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
