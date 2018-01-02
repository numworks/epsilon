#include <poincare/layout_engine.h>
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include<assert.h>
}

namespace Poincare {

ExpressionLayout * LayoutEngine::createInfixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  assert(numberOfOperands > 1);
  ExpressionLayout** children_layouts = new ExpressionLayout * [2*numberOfOperands-1];
  children_layouts[0] = expression->operand(0)->createLayout();
  for (int i=1; i<numberOfOperands; i++) {
    children_layouts[2*i-1] = new StringLayout(operatorName, 1);
    children_layouts[2*i] = expression->operand(i)->type() == Expression::Type::Opposite ? new ParenthesisLayout(expression->operand(i)->createLayout(floatDisplayMode, complexFormat), false) : expression->operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  /* HorizontalLayout holds the children layouts so they do not need to be
   * deleted here. */
  ExpressionLayout * layout = new HorizontalLayout(children_layouts, 2*numberOfOperands-1);
  delete[] children_layouts;
  return layout;
}

ExpressionLayout * LayoutEngine::createPrefixLayout(const Expression * expression, Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  ExpressionLayout ** grandChildrenLayouts = new ExpressionLayout *[2*numberOfOperands-1];
  int layoutIndex = 0;
  grandChildrenLayouts[layoutIndex++] = expression->operand(0)->createLayout(floatDisplayMode, complexFormat);
  for (int i = 1; i < numberOfOperands; i++) {
    grandChildrenLayouts[layoutIndex++] = new StringLayout(",", 1);
    grandChildrenLayouts[layoutIndex++] = expression->operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  /* HorizontalLayout holds the grand children layouts so they do not need to
   * be deleted */
  ExpressionLayout * argumentLayouts = new HorizontalLayout(grandChildrenLayouts, 2*numberOfOperands-1);
  delete [] grandChildrenLayouts;
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout(operatorName, strlen(operatorName));
  childrenLayouts[1] = new ParenthesisLayout(argumentLayouts, false);
  /* Same comment as above */
  return new HorizontalLayout(childrenLayouts, 2);
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
  assert(numberOfOperands > 1);
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
