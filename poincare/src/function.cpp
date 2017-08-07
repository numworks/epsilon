extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include <poincare/function.h>
#include <poincare/complex.h>
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Function::Function(const char * name, int requiredNumberOfArguments) :
  m_args(nullptr),
  m_numberOfArguments(0),
  m_requiredNumberOfArguments(requiredNumberOfArguments),
  m_name(name)
{
}

void Function::setArgument(Expression ** args, int numberOfArguments, bool clone) {
  build(args, numberOfArguments, clone);
}

void Function::setArgument(ListData * listData, bool clone) {
  build(listData->operands(), listData->numberOfOperands(), clone);
}

Function::~Function() {
  clean();
}

bool Function::hasValidNumberOfArguments() const {
  if (m_numberOfArguments != m_requiredNumberOfArguments) {
    return false;
  }
  for (int i = 0; i < m_requiredNumberOfArguments; i++) {
    if (!m_args[i]->hasValidNumberOfArguments()) {
      return false;
    }
  }
  return true;
}

const Expression * Function::operand(int i) const {
  assert(i >= 0 && i < m_numberOfArguments);
  return m_args[i];
}

int Function::numberOfOperands() const {
  return m_numberOfArguments;
}

Expression * Function::clone() const {
  return this->cloneWithDifferentOperands(m_args, m_numberOfArguments, true);
}

Complex Function::computeComplex(const Complex c, AngleUnit angleUnit) const {
  return Complex::Float(NAN);
}

Evaluation * Function::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  if (m_numberOfArguments != 1) {
    return new Complex(Complex::Float(NAN));
  }
  Evaluation * input = m_args[0]->evaluate(context, angleUnit);
  Complex * operands = new Complex[input->numberOfRows()*input->numberOfColumns()];
  for (int i = 0; i < input->numberOfOperands(); i++) {
    operands[i] = computeComplex(*input->complexOperand(i), angleUnit);
  }
  Evaluation * result = nullptr;
  if (input->numberOfOperands() == 1) {
    result = new Complex(operands[0]);
  } else {
    result = new ComplexMatrix(operands, input->numberOfRows(), input->numberOfColumns());
  }
  delete input;
  delete[] operands;
  return result;
}

ExpressionLayout * Function::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** grandChildrenLayouts = new ExpressionLayout *[2*m_numberOfArguments-1];
  int layoutIndex = 0;
  grandChildrenLayouts[layoutIndex++] = m_args[0]->createLayout(floatDisplayMode, complexFormat);
  for (int i = 1; i < m_numberOfArguments; i++) {
    grandChildrenLayouts[layoutIndex++] = new StringLayout(",", 1);
    grandChildrenLayouts[layoutIndex++] = m_args[i]->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * argumentLayouts = new HorizontalLayout(grandChildrenLayouts, 2*m_numberOfArguments-1);
  delete [] grandChildrenLayouts;
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout(m_name, strlen(m_name));
  childrenLayouts[1] = new ParenthesisLayout(argumentLayouts);
  return new HorizontalLayout(childrenLayouts, 2);
}

void Function::build(Expression ** args, int numberOfArguments, bool clone) {
  clean();
  m_numberOfArguments = numberOfArguments;
  m_args = new Expression * [numberOfArguments];
  for (int i = 0; i < numberOfArguments; i++) {
    assert(args[i] != nullptr);
    if (clone) {
      m_args[i] = args[i]->clone();
    } else {
      m_args[i] = args[i];
    }
  }
}

void Function::clean() {
    if (m_args != nullptr) {
    for (int i = 0; i < m_numberOfArguments; i++) {
      delete m_args[i];
    }
    delete[] m_args;
  }
}

}
