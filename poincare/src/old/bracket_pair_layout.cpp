#include <poincare/layout.h>
#include <poincare/old/layout_cursor.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

LayoutNode::DeletionMethod
BracketPairLayoutNode::deletionMethodForCursorLeftOfChild(
    int childIndex) const {
  return StandardDeletionMethodForLayoutContainingArgument(childIndex, 0);
}

size_t BracketPairLayoutNode::serializeWithSymbol(
    char symbolOpen, char symbolClose, char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  size_t length =
      SerializationHelper::CodePoint(buffer, bufferSize, symbolOpen);
  if (length >= bufferSize - 1) {
    return bufferSize - 1;
  }
  length +=
      childLayout()->serialize(buffer + length, bufferSize - length,
                               floatDisplayMode, numberOfSignificantDigits);
  if (length >= bufferSize) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize;
  }
  length += SerializationHelper::CodePoint(buffer + length, bufferSize - length,
                                           symbolClose);
  return std::min(length, bufferSize - 1);
}

}  // namespace Poincare
