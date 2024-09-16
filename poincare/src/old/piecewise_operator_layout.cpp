#include <poincare/layout.h>
#include <poincare/old/piecewise_operator.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

// SerializableNode
size_t PiecewiseOperatorLayoutNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  assert(numberOfColumns() == 2);
  int lastChildIndex = numberOfChildren() - 1;
  if (isEditing()) {
    lastChildIndex -= 2;
  }
  if (childAtIndex(lastChildIndex)->isEmpty()) {
    lastChildIndex--;
  }
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      PiecewiseOperator::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::Classic, lastChildIndex);
}

}  // namespace Poincare
