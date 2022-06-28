#ifndef POINCARE_LIST_TWO_PARAMETERS_FUNCTION_H
#define POINCARE_LIST_TWO_PARAMETERS_FUNCTION_H

#include <poincare/list.h>
#include <poincare/serialization_helper.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class ListTwoParametersFunctionNode : public ExpressionNode {
public:
  virtual const char * functionName() const = 0;

  int numberOfChildren() const override { return 2; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  bool canTakeDefaultParameterAtIndex(int i) const override { return i > 0; }
  Expression defaultParameterAtIndex(int i) const override {
    assert(canTakeDefaultParameterAtIndex(i));
    return List::DefaultWeightsList(Expression(this));
  }

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, functionName());
}

Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
  return LayoutHelper::Prefix(Expression(this), floatDisplayMode, numberOfSignificantDigits, functionName());
}

};

}

#endif
