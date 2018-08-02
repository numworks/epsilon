#include <poincare/layout_engine.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <poincare/vertical_offset_layout_node.h>
#include <assert.h>

namespace Poincare {

LayoutRef LayoutEngine::createInfixLayout(const Expression * expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  int numberOfChildren = expression->numberOfChildren();
  assert(numberOfChildren > 1);
  HorizontalLayoutRef result;
  result.addOrMergeChildAtIndex(expression->operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, 0);
  for (int i = 1; i < numberOfChildren; i++) {
    result.addOrMergeChildAtIndex(createStringLayout(operatorName, strlen(operatorName)), result.numberOfChildren(), true);
    result.addOrMergeChildAtIndex(
        expression->operand(i)->createLayout(floatDisplayMode, numberOfSignificantDigits),
        result.numberOfChildren(),
        true);
  }
  return result;
}

LayoutRef LayoutEngine::createPrefixLayout(const Expression * expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  HorizontalLayoutRef result;
  // Add the operator name.
  result.addOrMergeChildAtIndex(createStringLayout(operatorName, strlen(operatorName)), 0, true);

  // Create the layout of arguments separated by commas.
  HorizontalLayoutRef args;
  int numberOfChildren = expression->numberOfChildren();
  if (numberOfChildren > 0) {
    args.addOrMergeChildAtIndex(expression->operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
    for (int i = 1; i < numberOfChildren; i++) {
      args.addChildAtIndex(CharLayoutRef(','), args.numberOfChildren(), args.numberOfChildren(), nullptr);
      args.addOrMergeChildAtIndex(expression->operand(i)->createLayout(floatDisplayMode, numberOfSignificantDigits), args.numberOfChildren(), true);
    }
  }
  // Add the parenthesed arguments.
  result.addOrMergeChildAtIndex(createParenthesedLayout(args, false), result.numberOfChildren(), true);
  return result;
}

LayoutRef LayoutEngine::createParenthesedLayout(LayoutRef layoutRef, bool cloneLayout) {
  HorizontalLayoutRef result;
  result.addChildTreeAtIndex(LeftParenthesisLayoutRef(), 0, 0);
  if (layoutRef.isDefined()) {
    result.addOrMergeChildAtIndex(cloneLayout ? layoutRef.clone() : layoutRef, 1, true);
  }
  result.addChildTreeAtIndex(RightParenthesisLayoutRef(), result.numberOfChildren(), result.numberOfChildren());
  return result;
}

LayoutRef LayoutEngine::createStringLayout(const char * buffer, int bufferSize, KDText::FontSize fontSize) {
  assert(bufferSize > 0);
  HorizontalLayoutRef resultLayout;
  for (int i = 0; i < bufferSize; i++) {
    resultLayout.addChildAtIndex(CharLayoutRef(buffer[i], fontSize), i, i, nullptr);
  }
  return resultLayout;
}

LayoutRef LayoutEngine::createLogLayout(LayoutRef argument, LayoutRef index) {
  HorizontalLayoutRef resultLayout = HorizontalLayoutRef(createStringLayout("log", 3));
  VerticalOffsetLayoutRef offsetLayout = VerticalOffsetLayoutRef(index, VerticalOffsetLayoutNode::Type::Subscript);
  resultLayout.addChildAtIndex(offsetLayout, resultLayout.numberOfChildren(), resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(createParenthesedLayout(argument, false), resultLayout.numberOfChildren(), true);
  return resultLayout;
}

/* SerializableReference to Text */
int LayoutEngine::writeInfixSerializableRefTextInBuffer(
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

int LayoutEngine::writePrefixSerializableRefTextInBuffer(
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

void LayoutEngine::writeChildTreeInBuffer(SerializableRef childRef, SerializableRef parentRef, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfDigits, int * numberOfChar) {
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


int LayoutEngine::writeOneCharInBuffer(char * buffer, int bufferSize, char charToWrite) {
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
