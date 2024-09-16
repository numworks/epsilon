#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/sum_and_product.h>

#include <algorithm>
#include <array>

namespace Poincare {

// Protected

size_t SequenceLayoutNode::writeDerivedClassInBuffer(
    const char *operatorName, char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  assert(operatorName != nullptr);
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize - 1] = 0;

  // Write the operator name
  size_t numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }

  /* Add system parentheses to avoid serializing:
   *   2)+(1           2),1
   *    ∑     (5)  or   π    (5)
   *   n=1             n=1+binomial(3
   */
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar,
      UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }

  LayoutNode *argLayouts[] = {
      const_cast<SequenceLayoutNode *>(this)->argumentLayout(),
      const_cast<SequenceLayoutNode *>(this)->variableLayout(),
      const_cast<SequenceLayoutNode *>(this)->lowerBoundLayout(),
      const_cast<SequenceLayoutNode *>(this)->upperBoundLayout()};
  for (uint8_t i = 0; i < std::size(argLayouts); i++) {
    if (i != 0) {
      // Write the comma
      numberOfChar += SerializationHelper::CodePoint(
          buffer + numberOfChar, bufferSize - numberOfChar, ',');
      if (numberOfChar >= bufferSize - 1) {
        return bufferSize - 1;
      }
    }
    // Write the child with system parentheses
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar,
        UCodePointLeftSystemParenthesis);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
    numberOfChar += argLayouts[i]->serialize(
        buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
        numberOfSignificantDigits);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar,
        UCodePointRightSystemParenthesis);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }

  // Write the closing system parenthesis
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar,
      UCodePointRightSystemParenthesis);
  return numberOfChar;
}

}  // namespace Poincare
