#include <poincare/layout.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

// SerializableNode
size_t MatrixLayoutNode::serialize(char *buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }

  // Write the opening bracket
  size_t numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '[');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }

  /* Do not serialize the outmost lines if they are empty: compute the first and
   * last lines to serialize. */
  int minRow = 0;
  bool matrixIsEmpty = true;
  for (int i = 0; i < m_numberOfRows; i++) {
    if (!isRowEmpty(i)) {
      minRow = i;
      matrixIsEmpty = false;
      break;
    }
  }
  assert(m_numberOfRows > 0);
  int maxRow = m_numberOfRows - 1;
  if (!matrixIsEmpty) {
    for (int i = m_numberOfRows - 1; i >= 0; i--) {
      if (!isRowEmpty(i)) {
        maxRow = i;
        break;
      }
    }
  }

  // Serialize the vectors
  int maxColumn = isEditing() ? m_numberOfColumns - 2 : m_numberOfColumns - 1;
  for (int i = minRow; i <= maxRow; i++) {
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, '[');
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }

    numberOfChar += SerializationHelper::Infix(
        this, buffer + numberOfChar, bufferSize - numberOfChar,
        floatDisplayMode, numberOfSignificantDigits, ",", i * m_numberOfColumns,
        i * m_numberOfColumns + maxColumn);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }

    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, ']');
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }

  // Write the final closing bracket
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar, ']');
  return std::min(numberOfChar, bufferSize - 1);
}

}  // namespace Poincare
