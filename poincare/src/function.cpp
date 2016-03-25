extern "C" {
#include <stdlib.h>
}
#include <poincare/function.h>
#include "layout/function_layout.h"

Function::Function(Expression * arg, char* function_name, bool clone_operands) {
  m_arg = (Expression *)malloc(sizeof(Expression));
  m_function_name = function_name;
  if (clone_operands) {
    m_arg = arg->clone();
  } else {
    m_arg = arg;
  }
}

Function::~Function() {
  delete m_arg;
}

ExpressionLayout * Function::createLayout(ExpressionLayout * parent) {
  return new FunctionLayout(parent, m_function_name, m_arg);
}

Expression * Function::operand(int i) {
  if (i==0) {
    return m_arg;
  } else {
    return nullptr;
  }
}

int Function::numberOfOperands() {
  return 1;
}
