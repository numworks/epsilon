#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression_reference.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class ParenthesisNode : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ParenthesisNode); }
#if TREE_LOG
  const char * description() const override { return "Parenthesis";  }
#endif
  int numberOfChildren() const override { return 1; }

  // Properties
  Type type() const override { return Type::Parenthesis; }
  int polynomialDegree(char symbolName) const override;

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixSerializableRefTextInBuffer(ExpressionReference(this), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
  }
  // Simplification
  ExpressionReference shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;

  // Approximation
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
private:
 template<typename T> EvaluationReference<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class ParenthesisReference : public ExpressionReference {
public:
  ParenthesisReference(ExpressionReference exp) {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<ParenthesisNode>();
    m_identifier = node->identifier();
    addChildAtIndex(exp, 0, 0);
  }
};

}

#endif
