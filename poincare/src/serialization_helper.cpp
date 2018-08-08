#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

static void writeChildTreeInBuffer(SerializableRef childRef, SerializableRef parentRef, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits, int * numberOfChar) {
  // Write the child with parentheses if needed
  bool addParentheses = childRef.needsParenthesisWithParent(parentRef);
  if (addParentheses) {
    buffer[*numberOfChar] = '(';
    numberOfChar++;
    if (*numberOfChar >= bufferSize-1) {
      return;
    }
  }
  *numberOfChar += childRef.writeTextInBuffer(buffer + *numberOfChar, bufferSize - *numberOfChar, floatDisplayMode, numberOfDigits);
  if (*numberOfChar >= bufferSize-1) {
    return;
  }
  if (addParentheses) {
    buffer[*numberOfChar] = ')';
    numberOfChar++;
  }
}

int SerializationHelper::Infix(
    const SerializableRef serializableRef,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    int firstChildIndex,
    int lastChildIndex)
{
  assert(serializableRef.isDefined());

  // If buffer has size 0 or 1, put a zero if it fits and return
  if (bufferSize == 0) {
    return -1;
  }

  buffer[bufferSize-1] = 0; // Null-terminate the buffer
  if (bufferSize == 1) {
    return 0;
  }

  // Get some information on the SerializableRef
  int numberOfChar = 0;
  int numberOfChildren = serializableRef.numberOfChildren();
  assert(numberOfChildren > 0);

  // Write the first child, with parentheses if needed
  writeChildTreeInBuffer((const_cast<SerializableRef *>(&serializableRef))->serializableChildAtIndex(firstChildIndex), serializableRef, buffer, bufferSize, floatDisplayMode, numberOfDigits, &numberOfChar);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }
  // For all remaining children:
  int lastIndex = lastChildIndex < 0 ? numberOfChildren - 1 : lastChildIndex;
  for (int i = firstChildIndex + 1; i < lastIndex+1; i++) {
    // Write the operator
    numberOfChar += strlcpy(buffer+numberOfChar, operatorName, bufferSize-numberOfChar);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
    // Write the child, with parentheses if needed
    writeChildTreeInBuffer((const_cast<SerializableRef *>(&serializableRef))->serializableChildAtIndex(i), serializableRef, buffer, bufferSize, floatDisplayMode, numberOfDigits, &numberOfChar);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
  }

  // Null-terminate the buffer
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int SerializationHelper::Prefix(
    const SerializableRef serializableRef,
    char * buffer,
    int bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfDigits,
    const char * operatorName,
    bool writeFirstChild)
{
  assert(serializableRef.isDefined());

  // If buffer has size 0 or 1, put a zero if it fits and return
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
    if (bufferSize == 1) {
    return 0;
  }

  // Copy the operator name
  int numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  // Add the opening parenthese
  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  int childrenCount = serializableRef.numberOfChildren();
  if (childrenCount > 0) {
    if (!writeFirstChild) {
      assert(childrenCount > 1);
    }
    int firstChildIndex = writeFirstChild ? 0 : 1;

    // Write the first child
    numberOfChar += const_cast<SerializableRef *>(&serializableRef)->serializableChildAtIndex(firstChildIndex).writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfDigits);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }

    // Write the remaining children, separated with commas
    for (int i = firstChildIndex + 1; i < childrenCount; i++) {
      buffer[numberOfChar++] = ',';
      if (numberOfChar >= bufferSize-1) {
        return bufferSize-1;
      }
      numberOfChar += const_cast<SerializableRef *>(&serializableRef)->serializableChildAtIndex(i).writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfDigits);
      if (numberOfChar >= bufferSize-1) {
        return bufferSize-1;
      }
    }
  }

  // Add the closing parenthese
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int SerializationHelper::Char(char * buffer, int bufferSize, char charToWrite) {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) {
    return 0;
  }
  buffer[0] = charToWrite;
  buffer[1] = 0;
  return 1;
}

}
