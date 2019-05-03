#include <poincare/serialization_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

static bool checkBufferSize(char * buffer, int bufferSize, int * result) {
  // If buffer has size 0 or 1, put a zero if it fits and return
  if (bufferSize == 0) {
    *result = -1;
    return true;
  }

  buffer[bufferSize-1] = 0; // Null-terminate the buffer
  if (bufferSize == 1) {
    *result = 0;
    return true;
  }
  return false;
}

static int serializeChild(
    const TreeNode * childNode,
    const TreeNode * parentNode,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits)
{
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  int numberOfChar = 0;
  // Write the child with parentheses if needed
  bool addParentheses = parentNode->childNeedsParenthesis(childNode);
  if (addParentheses) {
    numberOfChar += UTF8Decoder::CodePointToChars('(', buffer+numberOfChar, bufferSize - numberOfChar);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
  }
  numberOfChar+= childNode->serialize(buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfDigits);
  if (numberOfChar >= bufferSize-1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize-1;
  }
  if (addParentheses) {
    numberOfChar += UTF8Decoder::CodePointToChars(')', buffer+numberOfChar, bufferSize - numberOfChar);
  }
  if (numberOfChar >= bufferSize-1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize-1;
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int SerializationHelper::Infix(
    const TreeNode * node,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    int firstChildIndex,
    int lastChildIndex)
{
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  // Get some information on the node
  int numberOfChar = 0;
  int numberOfChildren = node->numberOfChildren();
  assert(numberOfChildren > 0);

  // Write the first child, with parentheses if needed
  numberOfChar+= serializeChild(node->childAtIndex(firstChildIndex), node, buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfDigits);
  if (numberOfChar >= bufferSize-1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize-1;
  }
  // For all remaining children:
  int lastIndex = lastChildIndex < 0 ? numberOfChildren - 1 : lastChildIndex;
  for (int i = firstChildIndex + 1; i < lastIndex+1; i++) {
    // Write the operator
    numberOfChar += strlcpy(buffer+numberOfChar, operatorName, bufferSize-numberOfChar);
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize-1;
    }
    // Write the child, with parentheses if needed
    numberOfChar+= serializeChild(node->childAtIndex(i), node, buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfDigits);
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize-1;
    }
  }

  // Null-terminate the buffer
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int SerializationHelper::Prefix(
    const TreeNode * node,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    bool writeFirstChild)
{
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }

  // Copy the operator name
  int numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize-1;
  }

  // Add the opening parenthese
  numberOfChar += UTF8Decoder::CodePointToChars('(', buffer+numberOfChar, bufferSize - numberOfChar);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  int childrenCount = node->numberOfChildren();
  if (childrenCount > 0) {
    if (!writeFirstChild) {
      assert(childrenCount > 1);
    }
    int firstChildIndex = writeFirstChild ? 0 : 1;

    // Write the first child
    numberOfChar += node->childAtIndex(firstChildIndex)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfDigits);
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize-1;
    }

    // Write the remaining children, separated with commas
    for (int i = firstChildIndex + 1; i < childrenCount; i++) {
      numberOfChar += UTF8Decoder::CodePointToChars(',', buffer+numberOfChar, bufferSize - numberOfChar);
      if (numberOfChar >= bufferSize-1) {
        return bufferSize-1;
      }
      numberOfChar += node->childAtIndex(i)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfDigits);
      if (numberOfChar >= bufferSize-1) {
        assert(buffer[bufferSize - 1] == 0);
        return bufferSize-1;
      }
    }
  }

  // Add the closing parenthese
  numberOfChar += UTF8Decoder::CodePointToChars(')', buffer+numberOfChar, bufferSize - numberOfChar);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int SerializationHelper::CodePoint(char * buffer, int bufferSize, class CodePoint c) {
  {
    int result = 0;
    if (checkBufferSize(buffer, bufferSize, &result)) {
      return result;
    }
  }
  size_t size = UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  if (size <= bufferSize - 1) {
    buffer[size] = 0;
  } else {
    assert(size -1 == bufferSize - 1);
    buffer[--size] = 0;
  }
  return size;
}

}
