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

Complex Logarithm::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(std::log10(c.a()));
}

Evaluation * Logarithm::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  if (m_numberOfArguments == 1) {
    return Function::privateEvaluate(context, angleUnit);
  }
  Evaluation * x = m_args[0]->evaluate(context, angleUnit);
  Evaluation * n = m_args[1]->evaluate(context, angleUnit);
  if (x->numberOfRows() != 1 || x->numberOfColumns() != 1 || n->numberOfRows() != 1 || n->numberOfColumns() != 1) {
    return new Complex(Complex::Float(NAN));
  }
  Complex result = Fraction::compute(computeComplex(*(n->complexOperand(0)), angleUnit), computeComplex(*(x->complexOperand(0)), angleUnit));
  delete x;
  delete n;
  return new Complex(result);
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
