extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/function.h>
#include <poincare/complex.h>
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Function::Function(const char * name) :
  m_args(nullptr),
  m_numberOfArguments(0),
  m_name(name)
{
}

void Function::setArgument(Expression ** args, int numberOfArguments, bool clone) {
  if (m_args != nullptr) {
    for (int i = 0; i < m_numberOfArguments; i++) {
      delete m_args[i];
    }
    free(m_args);
  }
  m_numberOfArguments = numberOfArguments;
  m_args = (Expression **)malloc(numberOfArguments*sizeof(Expression *));
  for (int i = 0; i < numberOfArguments; i++) {
    assert(args[i] != nullptr);
    if (clone) {
      m_args[i] = args[i]->clone();
    } else {
      m_args[i] = args[i];
    }
  }
}

void Function::setArgument(ListData * listData, bool clone) {
  if (m_args != nullptr) {
    for (int i = 0; i < m_numberOfArguments; i++) {
      delete m_args[i];
    }
    free(m_args);
  }
  m_numberOfArguments = listData->numberOfOperands();
  m_args = (Expression **)malloc(m_numberOfArguments*sizeof(Expression *));
  for (int i = 0; i < m_numberOfArguments; i++) {
    if (clone) {
      m_args[i] = (Expression *)listData->operand(i)->clone();
    } else {
      m_args[i] = (Expression *)listData->operand(i);
    }
  }
}

Function::~Function() {
  if (m_args != nullptr) {
    for (int i = 0; i < m_numberOfArguments; i++) {
      delete m_args[i];
    }
    free(m_args);
  }
}

bool Function::isValidNumberOfArguments(int numberOfArguments) {
  /* By default, a function has one argument only */
  return (numberOfArguments == 1);
}

Expression * Function::clone() const {
  return this->cloneWithDifferentOperands(m_args, m_numberOfArguments, true);
}

ExpressionLayout * Function::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** grandChildrenLayouts = (ExpressionLayout **)malloc((2*m_numberOfArguments-1)*sizeof(ExpressionLayout *));
  int layoutIndex = 0;
  grandChildrenLayouts[layoutIndex++] = m_args[0]->createLayout(floatDisplayMode, complexFormat);
  for (int i = 1; i < m_numberOfArguments; i++) {
    grandChildrenLayouts[layoutIndex++] = new StringLayout(",", 1);
    grandChildrenLayouts[layoutIndex++] = m_args[i]->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * argumentLayouts = new HorizontalLayout(grandChildrenLayouts, 2*m_numberOfArguments-1);
  free(grandChildrenLayouts);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout(m_name, strlen(m_name));
  childrenLayouts[1] = new ParenthesisLayout(argumentLayouts);
  return new HorizontalLayout(childrenLayouts, 2);
}

const Expression * Function::operand(int i) const {
  assert(i >= 0 && i < m_numberOfArguments);
  return m_args[i];
}

int Function::numberOfOperands() const {
  return m_numberOfArguments;
}

Expression * Function::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  /* Default function evaluation works for reel function */
  return new Complex(Complex::Float(approximate(context, angleUnit)));
}

}
