#ifndef POINCARE_LIST_FUNCTION_WITH_UP_TO_TWO_PARAMETERS_H
#define POINCARE_LIST_FUNCTION_WITH_UP_TO_TWO_PARAMETERS_H

#include <poincare/expression_node_with_up_to_two_children.h>
#include <poincare/layout_helper.h>
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

  bool getChildrenIfNonEmptyList(Expression* memoizedChildren) const {
    memoizedChildren[0] = Expression(childAtIndex(0));
    memoizedChildren[1] =
        numberOfChildren() == 2
            ? Expression(childAtIndex(1))
            : List::Ones(memoizedChildren[0].numberOfChildren());
    return memoizedChildren[0].type() == ExpressionNode::Type::List &&
           memoizedChildren[1].type() == ExpressionNode::Type::List &&
           memoizedChildren[0].numberOfChildren() != 0 &&
           memoizedChildren[1].numberOfChildren() != 0;
  }

 private:
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(
        this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
        functionName());
  }
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override {
    return LayoutHelper::Prefix(Expression(this), floatDisplayMode,
                                numberOfSignificantDigits, functionName(),
                                context);
  }
};

}  // namespace Poincare

#endif
