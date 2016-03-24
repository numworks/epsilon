#include <poincare/function.h>
#include "layout/function_layout.h"

Function::~Function() {
  delete m_arg;
}

ExpressionLayout * Function::createLayout(ExpressionLayout * parent) {
  return new FunctionLayout(parent, m_function_name, m_arg);
}
