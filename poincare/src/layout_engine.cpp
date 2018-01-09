#include <poincare/layout_engine.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_left_layout.h"
#include "layout/parenthesis_right_layout.h"
#include "layout/vertical_offset_layout.h"
extern "C" {
#include<assert.h>
}

namespace Poincare {

ExpressionLayout * LayoutEngine::createInfixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  assert(numberOfOperands > 1);
  HorizontalLayout * result = new HorizontalLayout();
  result->addOrMergeChildAtIndex(expression->operand(0)->createLayout(), 0);
  for (int i = 1; i < numberOfOperands; i++) {
    result->addOrMergeChildAtIndex(createStringLayout(operatorName, strlen(operatorName)), result->numberOfChildren());
    result->addOrMergeChildAtIndex(
        expression->operand(i)->type() == Expression::Type::Opposite ?
          createParenthesedLayout(expression->operand(i)->createLayout(floatDisplayMode, complexFormat), false) :
          expression->operand(i)->createLayout(floatDisplayMode, complexFormat),
        result->numberOfChildren());
  }
  return result;
}

ExpressionLayout * LayoutEngine::createPrefixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  HorizontalLayout * result = new HorizontalLayout();

  // Add the operator name.
  result->addOrMergeChildAtIndex(createStringLayout(operatorName, strlen(operatorName)), 0);

  // Create the layout of arguments separated by commas.
  HorizontalLayout * args = new HorizontalLayout();
  args->addOrMergeChildAtIndex(expression->operand(0)->createLayout(floatDisplayMode, complexFormat), 0);
  for (int i = 1; i < numberOfOperands; i++) {
    args->addChildAtIndex(new CharLayout(','), args->numberOfChildren());
    args->addOrMergeChildAtIndex(expression->operand(i)->createLayout(floatDisplayMode, complexFormat), args->numberOfChildren());
  }

  // Add the parenthesed arguments.
  result->addOrMergeChildAtIndex(createParenthesedLayout(args, false), result->numberOfChildren());
  return result;
}

ExpressionLayout * LayoutEngine::createParenthesedLayout(ExpressionLayout * layout, bool cloneLayout) {
  HorizontalLayout * result = new HorizontalLayout();
  result->addChildAtIndex(new ParenthesisLeftLayout(), 0);
  if (layout != nullptr) {
    result->addOrMergeChildAtIndex(cloneLayout ? layout->clone() : layout, 1);
  }
  result->addChildAtIndex(new ParenthesisRightLayout(), result->numberOfChildren());
  return result;
}

ExpressionLayout * LayoutEngine::createStringLayout(const char * buffer, int bufferSize, KDText::FontSize fontSize) {
  assert(bufferSize > 0);
  HorizontalLayout * resultLayout = new HorizontalLayout();
  for (int i = 0; i < bufferSize; i++) {
    resultLayout->addChildAtIndex(new CharLayout(buffer[i], fontSize), i);
  }
  return resultLayout;
}

ExpressionLayout * LayoutEngine::createLogLayout(ExpressionLayout * argument, ExpressionLayout * index) {
  HorizontalLayout * resultLayout = static_cast<HorizontalLayout *>(createStringLayout("log", 3));
  VerticalOffsetLayout * offsetLayout = new VerticalOffsetLayout(index, VerticalOffsetLayout::Type::Subscript, false);
  resultLayout->addChildAtIndex(offsetLayout, resultLayout->numberOfChildren());
  resultLayout->addOrMergeChildAtIndex(createParenthesedLayout(argument, false), resultLayout->numberOfChildren());
  return resultLayout;
}

int LayoutEngine::writeInfixExpressionTextInBuffer(const Expression * expression, char * buffer, int bufferSize, const char * operatorName, OperandNeedsParenthesis operandNeedsParenthesis) {
  return writeInfixExpressionOrExpressionLayoutTextInBuffer(expression, nullptr, buffer, bufferSize, operatorName, 0, -1, operandNeedsParenthesis, [](const char * operatorName) { return true; });
}

int LayoutEngine::writePrefixExpressionTextInBuffer(const Expression * expression, char * buffer, int bufferSize, const char * operatorName) {
  return writePrefixExpressionOrExpressionLayoutTextInBuffer(expression, nullptr, buffer, bufferSize, operatorName);
}

int LayoutEngine::writeInfixExpressionLayoutTextInBuffer(const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, const char * operatorName, int firstChildIndex, int lastChildIndex, ChildNeedsParenthesis childNeedsParenthesis) {
  return writeInfixExpressionOrExpressionLayoutTextInBuffer(nullptr, expressionLayout, buffer, bufferSize, operatorName, firstChildIndex, lastChildIndex, [](const Expression * e) { return true; }, childNeedsParenthesis);
}

int LayoutEngine::writePrefixExpressionLayoutTextInBuffer(const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, const char * operatorName, bool writeFirstChild) {
  return writePrefixExpressionOrExpressionLayoutTextInBuffer(nullptr, expressionLayout, buffer, bufferSize, operatorName, writeFirstChild);
}

int LayoutEngine::writeInfixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, const char * operatorName, int firstChildIndex, int lastChildIndex, OperandNeedsParenthesis operandNeedsParenthesis, ChildNeedsParenthesis childNeedsParenthesis) {
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

  if ((expression != nullptr && operandNeedsParenthesis(expression->operand(firstChildIndex)))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
  {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) {
      return bufferSize-1;
    }
  }

  numberOfChar += (expression != nullptr) ? expression->operand(firstChildIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar) : expressionLayout->child(firstChildIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if ((expression != nullptr && operandNeedsParenthesis(expression->operand(firstChildIndex)))
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
    if ((expression != nullptr && operandNeedsParenthesis(expression->operand(i)))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
    {
      buffer[numberOfChar++] = '(';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
    numberOfChar += (expression != nullptr) ? expression->operand(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar) : expressionLayout->child(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
    if ((expression != nullptr && operandNeedsParenthesis(expression->operand(i)))
      || (expression == nullptr && childNeedsParenthesis(operatorName)))
    {
      buffer[numberOfChar++] = ')';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}


int LayoutEngine::writePrefixExpressionOrExpressionLayoutTextInBuffer(const Expression * expression, const ExpressionLayout * expressionLayout, char * buffer, int bufferSize, const char * operatorName, bool writeFirstChild) {
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
  assert(numberOfOperands > 0);
  if (!writeFirstChild) {
    assert(numberOfOperands > 1);
  }
  int firstOperandIndex = writeFirstChild ? 0 : 1;
  numberOfChar += (expression != nullptr) ? expression->operand(firstOperandIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar) : expressionLayout->child(firstOperandIndex)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  for (int i = firstOperandIndex + 1; i < numberOfOperands; i++) {
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    buffer[numberOfChar++] = ',';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += (expression != nullptr) ? expression->operand(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar) : expressionLayout->child(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  }
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
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
