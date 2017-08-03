#include <poincare/logarithm.h>
#include <poincare/fraction.h>
#include <cmath>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/baseline_relative_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Logarithm::Logarithm() :
  Function("log", 2)
{
}

bool Logarithm::hasValidNumberOfArguments() const {
  if (m_numberOfArguments != 1 && m_numberOfArguments != 2) {
    return false;
  }
  for (int i = 0; i < m_numberOfArguments; i++) {
    if (!m_args[i]->hasValidNumberOfArguments()) {
      return false;
    }
  }
  return true;
}

Expression::Type Logarithm::type() const {
  return Type::Logarithm;
}

Expression * Logarithm::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Logarithm * l = new Logarithm();
  l->setArgument(newOperands, numberOfOperands, cloneOperands);
  return l;
}

template<typename T>
Complex<T> Logarithm::templatedComputeComplex(const Complex<T> c) const {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::log10(c.a()));
}

template<typename T>
Evaluation<T> * Logarithm::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  if (m_numberOfArguments == 1) {
    return Function::templatedEvaluate<T>(context, angleUnit);
  }
  Evaluation<T> * x = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * n = m_args[1]->evaluate<T>(context, angleUnit);
  if (x->numberOfRows() != 1 || x->numberOfColumns() != 1 || n->numberOfRows() != 1 || n->numberOfColumns() != 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> result = Fraction::compute<T>(templatedComputeComplex(*(n->complexOperand(0))), templatedComputeComplex(*(x->complexOperand(0))));
  delete x;
  delete n;
  return new Complex<T>(result);
}

ExpressionLayout * Logarithm::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  if (m_numberOfArguments == 1) {
    return Function::privateCreateLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new BaselineRelativeLayout(new StringLayout(m_name, strlen(m_name)), m_args[0]->createLayout(floatDisplayMode, complexFormat), BaselineRelativeLayout::Type::Subscript);
  childrenLayouts[1] = new ParenthesisLayout(m_args[1]->createLayout(floatDisplayMode, complexFormat));
  return new HorizontalLayout(childrenLayouts, 2);
}

}
