#ifndef POINCARE_EMPTY_EXPRESSION_H
#define POINCARE_EMPTY_EXPRESSION_H

#include "old_expression.h"

namespace Poincare {

// An empty expression awaits completion by the user.

class EmptyExpressionNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(EmptyExpressionNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "EmptyExpression";
  }
#endif

  // Properties
  Type otype() const override { return Type::EmptyExpression; }
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  OExpression removeUnit(OExpression* unit) override {
    assert(false);
    return ExpressionNode::removeUnit(unit);
  }

  // Simplification
  LayoutShape leftLayoutShape() const override {
    // leftLayoutShape of EmptyExpression is only called from Conjugate
    assert(parent() && parent()->otype() == Type::Conjugate);
    return LayoutShape::OneLetter;
  };
};

class EmptyExpression final : public OExpression {
 public:
  static EmptyExpression Builder() {
    return PoolHandle::FixedArityBuilder<EmptyExpression,
                                         EmptyExpressionNode>();
  }
  EmptyExpression(const EmptyExpressionNode* n) : OExpression(n) {}
};

}  // namespace Poincare

#endif
