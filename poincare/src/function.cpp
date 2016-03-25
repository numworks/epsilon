extern "C" {
#include <stdlib.h>
}
#include <poincare/function.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

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

ExpressionLayout * Function::createLayout() {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(4*sizeof(ExpressionLayout *));
  children_layouts[0] = new StringLayout(m_function_name, strlen(m_function_name));
  char string[2] = {'(', '\0'};
  children_layouts[1] = new StringLayout(string, 1);
  children_layouts[2] = m_arg->createLayout();
  string[0] = ')';
  children_layouts[3] = new StringLayout(string, 1);
  return new HorizontalLayout(children_layouts, 4);
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
