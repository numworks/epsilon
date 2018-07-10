#include <poincare/layout_engine.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/left_parenthesis_layout.h"
#include "layout/right_parenthesis_layout.h"
#include "layout/vertical_offset_layout.h"
#include <poincare/horizontal_layout_node.h>
#include <poincare/char_layout_node.h>

extern "C" {
#include<assert.h>
}

namespace Poincare {

LayoutRef LayoutEngine::createInfixLayout(const Expression * expression, PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  assert(numberOfOperands > 1);
  HorizontalLayoutRef result;
  result.addChildTreeAtIndex(expression->operand(0)->createLayout(), 0);
  for (int i = 1; i < numberOfOperands; i++) {
    result.addChildTreeAtIndex(createStringLayout(operatorName, strlen(operatorName)), result.numberOfChildren());
    result.addChildTreeAtIndex(
        expression->operand(i)->createLayout(floatDisplayMode, complexFormat),
        result.numberOfChildren());
  }
    /*
  result->addOrMergeChildAtIndex(expression->operand(0)->createLayout(), 0, true);
  for (int i = 1; i < numberOfOperands; i++) {
    result->addOrMergeChildAtIndex(createStringLayout(operatorName, strlen(operatorName)), result->numberOfChildren(), true);
    result->addOrMergeChildAtIndex(
        expression->operand(i)->type() == Expression::Type::Opposite ?
          createParenthesedLayout(expression->operand(i)->createLayout(floatDisplayMode, complexFormat), false) :
          expression->operand(i)->createLayout(floatDisplayMode, complexFormat),
        result->numberOfChildren(), true);
  }*/
  return result;
}

LayoutRef LayoutEngine::createPrefixLayout(const Expression * expression, PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  HorizontalLayoutRef result;
  // Add the operator name.
  result.addChildTreeAtIndex(createStringLayout(operatorName, strlen(operatorName)), 0);

  // Create the layout of arguments separated by commas.
  HorizontalLayoutRef args;
  int numberOfOperands = expression->numberOfOperands();
  if (numberOfOperands > 0) {
    args.addChildTreeAtIndex(expression->operand(0)->createLayout(floatDisplayMode, complexFormat), 0);
    for (int i = 1; i < numberOfOperands; i++) {
      args.addChildTreeAtIndex(CharLayoutRef(','), args.numberOfChildren());
      args.addChildTreeAtIndex(expression->operand(i)->createLayout(floatDisplayMode, complexFormat), args.numberOfChildren());
    }
  }
  // Add the parenthesed arguments.
  result.addChildTreeAtIndex(createParenthesedLayout(args, false), result.numberOfChildren());
  /*// Add the operator name.
  result->addOrMergeChildAtIndex(createStringLayout(operatorName, strlen(operatorName)), 0, true);

  // Create the layout of arguments separated by commas.
  ExpressionLayout * args = nullptr;
  int numberOfOperands = expression->numberOfOperands();
  if (numberOfOperands > 0) {
    args = new HorizontalLayout();
    HorizontalLayout * horizontalArgs = static_cast<HorizontalLayout *>(args);
    horizontalArgs->addOrMergeChildAtIndex(expression->operand(0)->createLayout(floatDisplayMode, complexFormat), 0, true);
    for (int i = 1; i < numberOfOperands; i++) {
      horizontalArgs->addChildTreeAtIndex(new CharLayout(','), args->numberOfChildren());
      horizontalArgs->addOrMergeChildAtIndex(expression->operand(i)->createLayout(floatDisplayMode, complexFormat), horizontalArgs->numberOfChildren(), true);
    }
  }
  // Add the parenthesed arguments.
  result->addOrMergeChildAtIndex(createParenthesedLayout(args, false), result->numberOfChildren(), true);*/
  return result;
}

LayoutRef LayoutEngine::createParenthesedLayout(LayoutRef layoutRef, bool cloneLayout) {
  HorizontalLayoutRef result;
  result.addChildTreeAtIndex(CharLayoutRef('('), 0);
  if (layoutRef.isDefined()) {
    result.addChildTreeAtIndex(cloneLayout ? layoutRef.clone() : layoutRef, 1);
  }
  result.addChildTreeAtIndex(CharLayoutRef(')'), result.numberOfChildren());
  return result;
}

LayoutRef LayoutEngine::createStringLayout(const char * buffer, int bufferSize, KDText::FontSize fontSize) {
  assert(bufferSize > 0);
  HorizontalLayoutRef resultLayout;
  for (int i = 0; i < bufferSize; i++) {
    resultLayout.addChildTreeAtIndex(CharLayoutRef(buffer[i], fontSize), i);
  }
  return resultLayout;
}

ExpressionLayout * LayoutEngine::createLogLayout(ExpressionLayout * argument, ExpressionLayout * index) {
  return nullptr;
  /*HorizontalLayout * resultLayout = static_cast<HorizontalLayout *>(createStringLayout("log", 3));
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(index, VerticalOffsetLayout::Type::Subscript, false);
  resultLayout->addChildAtIndex(offsetLayout, resultLayout->numberOfChildren());
  resultLayout->addOrMergeChildAtIndex(createParenthesedLayout(argument, false), resultLayout->numberOfChildren(), true);
  return resultLayout;*/
}

int LayoutEngine::writeInfixExpressionTextInBuffer(const Expression * expression, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName) {
  return writeInfixExpressionOrExpressionLayoutTextInBuffer(expression, nullptr, buffer, bufferSize, numberOfDigits, operatorName, 0, -1, [](const char * operatorName) { return true; });
}

int LayoutEngine::writePrefixExpressionTextInBuffer(const Expression * expression, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName) {
  return writePrefixExpressionOrExpressionLayoutTextInBuffer(expression, nullptr, buffer, bufferSize, numberOfDigits, operatorName);
}

int LayoutEngine::writeInfixExpressionLayoutTextInBuffer(const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName, int firstChildIndex, int lastChildIndex, ChildNeedsParenthesis childNeedsParenthesis) {
  return writeInfixExpressionOrExpressionLayoutTextInBuffer(nullptr, expressionLayout, buffer, bufferSize, numberOfDigits, operatorName, firstChildIndex, lastChildIndex, childNeedsParenthesis);
}

int LayoutEngine::writePrefixExpressionLayoutTextInBuffer(const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName, bool writeFirstChild) {
  return writePrefixExpressionOrExpressionLayoutTextInBuffer(nullptr, expressionLayout, buffer, bufferSize, numberOfDigits, operatorName, writeFirstChild);
}

int LayoutEngine::writeInfixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName, int firstChildIndex, int lastChildIndex, ChildNeedsParenthesis childNeedsParenthesis) {
  assert(expression != nullptr || expressionLayout != nullptr);
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  int numberOfOperands = (expression != nullptr) ? expression->numberOfOperands() : expressionLayout->numberOfChildren();
  assert(numberOfOperands > 0);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  if ((expression != nullptr && expression->operand(firstChildIndex)->needParenthesisWithParent(expression))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
  {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
  }

  numberOfChar += (expression != nullptr) ? expression->operand(firstChildIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits) : expressionLayout->child(firstChildIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  if ((expression != nullptr && expression->operand(firstChildIndex)->needParenthesisWithParent(expression))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
  {
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  int lastIndex = lastChildIndex < 0 ? numberOfOperands - 1 : lastChildIndex;
  for (int i = firstChildIndex + 1; i < lastIndex+1; i++) {
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += strlcpy(buffer+numberOfChar, operatorName, bufferSize-numberOfChar);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    if ((expression != nullptr && expression->operand(i)->needParenthesisWithParent(expression))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
    {
      buffer[numberOfChar++] = '(';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
    numberOfChar += (expression != nullptr) ? expression->operand(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits) : expressionLayout->child(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    if ((expression != nullptr && expression->operand(i)->needParenthesisWithParent(expression))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
    {
      buffer[numberOfChar++] = ')';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int LayoutEngine::writePrefixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName, bool writeFirstChild) {
  assert(expression != nullptr || expressionLayout != nullptr);
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  int numberOfOperands = (expression != nullptr) ? expression->numberOfOperands() : expressionLayout->numberOfChildren();
  if (numberOfOperands > 0) {
    if (!writeFirstChild) {
      assert(numberOfOperands > 1);
    }
    int firstOperandIndex = writeFirstChild ? 0 : 1;
    numberOfChar += (expression != nullptr) ? expression->operand(firstOperandIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits) : expressionLayout->child(firstOperandIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    for (int i = firstOperandIndex + 1; i < numberOfOperands; i++) {
      buffer[numberOfChar++] = ',';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
      numberOfChar += (expression != nullptr) ? expression->operand(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar) : expressionLayout->child(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
  }
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

/* LayoutReference to Text */
int LayoutEngine::writeInfixSerializableRefTextInBuffer(
    const SerializableRef serializableRef,
    char * buffer,
    int bufferSize,
    int numberOfDigits,
    const char * operatorName,
    int firstChildIndex,
    int lastChildIndex)
{
  // If buffer has size 0 or 1, put a zero if it fits and return
  assert(serializableRef.isDefined());
  if (bufferSize == 0) {
    return -1;
  }

  buffer[bufferSize-1] = 0; // Null-terminate the buffer
  if (bufferSize == 1) {
    return 0;
  }

  // Get some information on the SerializableRef
  int numberOfChar = 0;
  int numberOfOperands = serializableRef.numberOfChildren();
  assert(numberOfOperands > 0);

  // Write the first child, with parentheses if needed
  writeChildTreeInBuffer((const_cast<SerializableRef *>(&serializableRef))->serializableChildAtIndex(firstChildIndex), serializableRef, buffer, bufferSize, numberOfDigits, &numberOfChar);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }
  // For all remaining children:
  int lastIndex = lastChildIndex < 0 ? numberOfOperands - 1 : lastChildIndex;
  for (int i = firstChildIndex + 1; i < lastIndex+1; i++) {
    // Write the operator
    numberOfChar += strlcpy(buffer+numberOfChar, operatorName, bufferSize-numberOfChar);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
    // Write the child, with parentheses if needed
    writeChildTreeInBuffer((const_cast<SerializableRef *>(&serializableRef))->serializableChildAtIndex(i), serializableRef, buffer, bufferSize, numberOfDigits, &numberOfChar);
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
  }

  // Null-terminate the buffer
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void LayoutEngine::writeChildTreeInBuffer(SerializableRef childRef, SerializableRef parentRef, char * buffer, int bufferSize, int numberOfDigits, int * numberOfChar) {
  // Write the child with parentheses if needed
  bool addParentheses = childRef.needsParenthesisWithParent(parentRef);
  if (addParentheses) {
    buffer[*numberOfChar++] = '('; //TODO ok ?
    if (*numberOfChar >= bufferSize-1) {
      return;
    }
  }
  *numberOfChar += childRef.writeTextInBuffer(buffer + *numberOfChar, bufferSize - *numberOfChar, numberOfDigits);
  if (*numberOfChar >= bufferSize-1) {
    return;
  }
  if (addParentheses) {
    buffer[*numberOfChar++] = ')';
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
