#include <assert.h>
#include <escher/metric.h>
#include <poincare/layout.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

#define Layout OLayout

size_t FractionLayoutNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }

  /* Add System parenthesis to detect omitted multiplication:
   *   2
   *  --- i --> [2/3]i instead of 2/3i
   *   3
   */

  // Add system parenthesis
  size_t numberOfChar = SerializationHelper::CodePoint(
      buffer, bufferSize, UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }

  // Write the content of the fraction
  numberOfChar += SerializationHelper::Infix(
      this, buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
      numberOfSignificantDigits, "/");
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }

  // Add system parenthesis
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar,
      UCodePointRightSystemParenthesis);
  return numberOfChar;
}

bool FractionLayoutNode::isCollapsable(
    int* numberOfOpenParenthesis, OMG::HorizontalDirection direction) const {
  if (*numberOfOpenParenthesis > 0) {
    return true;
  }

  /* We do not want to absorb a fraction if something else is already being
   * absorbed. This way, the user can write a product of fractions without
   * typing the × sign. */
  Layout p = Layout(parent());
  assert(!p.isUninitialized() && p.isHorizontal() && p.numberOfChildren() > 1);
  int indexInParent = p.indexOfChild(Layout(this));
  int indexOfAbsorbingSibling = indexInParent + (direction.isLeft() ? 1 : -1);
  assert(indexOfAbsorbingSibling >= 0 &&
         indexOfAbsorbingSibling < p.numberOfChildren());
  Layout absorbingSibling = p.childAtIndex(indexOfAbsorbingSibling);
  if (absorbingSibling.numberOfChildren() > 0) {
    absorbingSibling = absorbingSibling.childAtIndex(
        direction.isLeft()
            ? absorbingSibling.leftCollapsingAbsorbingChildIndex()
            : absorbingSibling.rightCollapsingAbsorbingChildIndex());
  }
  return absorbingSibling.isHorizontal() && absorbingSibling.isEmpty();
}

}  // namespace Poincare
