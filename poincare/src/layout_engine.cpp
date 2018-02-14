#include <poincare/layout_engine.h>
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include<assert.h>
}

namespace Poincare {

ExpressionLayout * LayoutEngine::createInfixLayout(const Expression * expression, PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  assert(numberOfOperands > 1);
  ExpressionLayout** children_layouts = new ExpressionLayout * [2*numberOfOperands-1];
  children_layouts[0] = expression->operand(0)->createLayout();
  for (int i=1; i<numberOfOperands; i++) {
    children_layouts[2*i-1] = new StringLayout(operatorName, 1);
    children_layouts[2*i] = expression->operand(i)->type() == Expression::Type::Opposite ? new ParenthesisLayout(expression->operand(i)->createLayout(floatDisplayMode, complexFormat)) : expression->operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  /* HorizontalLayout holds the children layouts so they do not need to be
   * deleted here. */
  ExpressionLayout * layout = new HorizontalLayout(children_layouts, 2*numberOfOperands-1);
  delete[] children_layouts;
  return layout;
}

ExpressionLayout * LayoutEngine::createPrefixLayout(const Expression * expression, PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat, const char * operatorName) {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != Expression::ComplexFormat::Default);
  int numberOfOperands = expression->numberOfOperands();
  ExpressionLayout * argumentLayouts = nullptr;
  if (numberOfOperands > 0) {
    ExpressionLayout ** grandChildrenLayouts = new ExpressionLayout *[2*numberOfOperands-1];
    int layoutIndex = 0;
    grandChildrenLayouts[layoutIndex++] = expression->operand(0)->createLayout(floatDisplayMode, complexFormat);
    for (int i = 1; i < numberOfOperands; i++) {
      grandChildrenLayouts[layoutIndex++] = new StringLayout(",", 1);
      grandChildrenLayouts[layoutIndex++] = expression->operand(i)->createLayout(floatDisplayMode, complexFormat);
    }
    /* HorizontalLayout holds the grand children layouts so they do not need to
     * be deleted */
    argumentLayouts = new HorizontalLayout(grandChildrenLayouts, 2*numberOfOperands-1);
    delete [] grandChildrenLayouts;
  }
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout(operatorName, strlen(operatorName));
  childrenLayouts[1] = new ParenthesisLayout(argumentLayouts);
  /* Same comment as above */
  return new HorizontalLayout(childrenLayouts, 2);
}

int LayoutEngine::writeInfixExpressionTextInBuffer(const Expression * expression, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName) {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  int numberOfOperands = expression->numberOfOperands();
  assert(numberOfOperands > 1);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  if (expression->operand(0)->needParenthesisWithParent(expression)) {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  numberOfChar += expression->operand(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
  if (expression->operand(0)->needParenthesisWithParent(expression)) {
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  for (int i=1; i<numberOfOperands; i++) {
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += strlcpy(buffer+numberOfChar, operatorName, bufferSize-numberOfChar);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    if (expression->operand(i)->needParenthesisWithParent(expression)) {
      buffer[numberOfChar++] = '(';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
    numberOfChar += expression->operand(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
    if (expression->operand(i)->needParenthesisWithParent(expression)) {
      buffer[numberOfChar++] = ')';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

int LayoutEngine::writePrefixExpressionTextInBuffer(const Expression * expression, char * buffer, int bufferSize, int numberOfDigits, const char * operatorName) {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  numberOfChar += strlcpy(buffer, operatorName, bufferSize);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  int numberOfOperands = expression->numberOfOperands();
  if (numberOfOperands > 0) {
    numberOfChar += expression->operand(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
    for (int i = 1; i < numberOfOperands; i++) {
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
      buffer[numberOfChar++] = ',';
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
      numberOfChar += expression->operand(i)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfDigits);
    }
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

}
