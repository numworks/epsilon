#include <poincare/commutative_operation.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

CommutativeOperation::CommutativeOperation(Expression ** operands, int numberOfOperands, bool cloneOperands) {
  assert(operands != nullptr);
  assert(numberOfOperands >= 2);
  m_numberOfOperands = numberOfOperands;
  m_operands = (Expression **)malloc(numberOfOperands*sizeof(Expression *));
  for (int i=0; i<numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
  }
}

CommutativeOperation::~CommutativeOperation() {
  for (int i=0; i<m_numberOfOperands; i++) {
    delete m_operands[i];
  }
  free(m_operands);
}

int CommutativeOperation::numberOfOperands() {
  return m_numberOfOperands;
}

Expression * CommutativeOperation::operand(int i) {
  assert(i >= 0);
  assert(i < m_numberOfOperands);
  return m_operands[i];
}

Expression * CommutativeOperation::clone() {
  return this->cloneWithDifferentOperands(m_operands, m_numberOfOperands, true);
}

float CommutativeOperation::approximate(Context& context) {
  float result = m_operands[0]->approximate(context);
  for (size_t i=1; i<m_numberOfOperands; i++) {
    float next = m_operands[i]->approximate(context);
    result = this->operateApproximatevelyOn(result, next);
  }
  return result;
}

ExpressionLayout * CommutativeOperation::createLayout() {
  int number_of_children = 2*m_numberOfOperands-1;
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(number_of_children*sizeof(ExpressionLayout *));
  char string[2] = {operatorChar(), '\0'};
  children_layouts[0] = m_operands[0]->createLayout();
  for (int i=1; i<m_numberOfOperands; i++) {
    children_layouts[2*i-1] = new StringLayout(string, 1);
    children_layouts[2*i] = m_operands[i]->createLayout();
  }
  return new HorizontalLayout(children_layouts, number_of_children);
}

bool CommutativeOperation::isCommutative() {
  return true;
}
