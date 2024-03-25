#ifndef POINCARE_LIST_FUNCTION_WITH_UP_TO_TWO_PARAMETERS_H
#define POINCARE_LIST_FUNCTION_WITH_UP_TO_TWO_PARAMETERS_H

#include <poincare/expression_node_with_up_to_two_children.h>
#include <poincare/list.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class ListFunctionWithOneOrTwoParametersNode
    : public ExpressionNodeWithOneOrTwoChildren {
 public:
  virtual const char* functionName() const = 0;

  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  bool getChildrenIfNonEmptyList(OExpression* memoizedChildren) const {
    memoizedChildren[0] = OExpression(childAtIndex(0));
    memoizedChildren[1] =
        numberOfChildren() == 2
            ? OExpression(childAtIndex(1))
            : OList::Ones(memoizedChildren[0].numberOfChildren());
    return memoizedChildren[0].otype() == ExpressionNode::Type::OList &&
           memoizedChildren[1].otype() == ExpressionNode::Type::OList &&
           memoizedChildren[0].numberOfChildren() != 0 &&
           memoizedChildren[1].numberOfChildren() != 0;
  }

 private:
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(
        this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
        functionName());
  }
};

}  // namespace Poincare

#endif
