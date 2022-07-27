#ifndef POINCARE_LIST_FUNCTION_WITH_UP_TO_TWO_PARAMETERS_H
#define POINCARE_LIST_FUNCTION_WITH_UP_TO_TWO_PARAMETERS_H

#include <poincare/list.h>
#include <poincare/serialization_helper.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class ListFunctionWithUpToTwoParametersNode : public ExpressionNode {
public:
  virtual const char * functionName() const = 0;

  int numberOfChildren() const override { return m_hasTwoChildren ? 2 : 1; }
  void setNumberOfChildren(int numberOfChildren) override {
    if (numberOfChildren == 1 || numberOfChildren <= 2) {
      m_hasTwoChildren = numberOfChildren == 2;
    }
  }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  bool getChildrenIfNonEmptyList(Expression * memoizedChildren) const {
    memoizedChildren[0] = Expression(childAtIndex(0));
    memoizedChildren[1] = m_hasTwoChildren ? Expression(childAtIndex(1)) : List::Ones(memoizedChildren[0].numberOfChildren());
    return memoizedChildren[0].type() == ExpressionNode::Type::List
        && memoizedChildren[1].type() == ExpressionNode::Type::List
        && memoizedChildren[0].numberOfChildren() != 0
        && memoizedChildren[1].numberOfChildren() != 0;
  }

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, functionName());
  }
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Prefix(Expression(this), floatDisplayMode, numberOfSignificantDigits, functionName());
  }
  bool m_hasTwoChildren;
};

}

#endif
