extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/function.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

Function::Function(const char * name) :
  m_name(name),
  m_arg(nullptr)
{
}

void Function::setArgument(Expression * arg, bool clone) {
  if (m_arg != nullptr) {
    delete m_arg;
  }
  if (clone) {
    m_arg = arg->clone();
  } else {
    m_arg = arg;
  }
}

Function::~Function() {
  if (m_arg != nullptr) {
    delete m_arg;
  }
}

Expression * Function::clone() const {
  return this->cloneWithDifferentOperands((Expression**)&m_arg, 1, true);
}

ExpressionLayout * Function::createLayout() const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(4*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new StringLayout(m_name, strlen(m_name));
  childrenLayouts[1] = new StringLayout("(", 1);
  childrenLayouts[2] = m_arg->createLayout();
  childrenLayouts[3] = new StringLayout(")", 1);
  return new HorizontalLayout(childrenLayouts, 4);
}

const Expression * Function::operand(int i) const {
  assert(i==0);
  return m_arg;
}

int Function::numberOfOperands() const {
  return 1;
}
