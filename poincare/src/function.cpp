extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/function.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

Function::Function(Expression * arg, char* functionName, bool cloneOperands) {
  assert(arg != nullptr);
  m_arg = (Expression *)malloc(sizeof(Expression));
  m_functionName = functionName;
  if (cloneOperands) {
    m_arg = arg->clone();
  } else {
    m_arg = arg;
  }
}

Function::~Function() {
  delete m_arg;
}

Expression * Function::clone() const {
  return this->cloneWithDifferentOperands((Expression**)&m_arg, 1, true);
}

ExpressionLayout * Function::createLayout() const {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(4*sizeof(ExpressionLayout *));
  children_layouts[0] = new StringLayout(m_functionName, strlen(m_functionName));
  char string[2] = {'(', '\0'};
  children_layouts[1] = new StringLayout(string, 1);
  children_layouts[2] = m_arg->createLayout();
  string[0] = ')';
  children_layouts[3] = new StringLayout(string, 1);
  return new HorizontalLayout(children_layouts, 4);
}

const Expression * Function::operand(int i) const {
  if (i==0) {
    return m_arg;
  } else {
    return nullptr;
  }
}

int Function::numberOfOperands() const {
  return 1;
}
