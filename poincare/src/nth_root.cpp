#include <poincare/nth_root.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
#include <poincare/power.h>
#include "layout/nth_root_layout.h"

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

NthRoot::NthRoot() :
  Function("root")
{
}

bool NthRoot::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 2);
}

Expression::Type NthRoot::type() const {
  return Type::NthRoot;
}

Expression * NthRoot::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  NthRoot * r = new NthRoot();
  r->setArgument(newOperands, numberOfOperands, cloneOperands);
  return r;
}

float NthRoot::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return powf(m_args[0]->approximate(context, angleUnit), 1.0f/m_args[1]->approximate(context, angleUnit));
}

ExpressionLayout * NthRoot::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new NthRootLayout(m_args[0]->createLayout(floatDisplayMode, complexFormat), m_args[1]->createLayout(floatDisplayMode, complexFormat));
}

Expression * NthRoot::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * baseEvaluation = m_args[0]->evaluate(context, angleUnit);
  Expression * indexEvaluation = m_args[1]->evaluate(context, angleUnit);
  assert(baseEvaluation->type() == Type::Matrix || baseEvaluation->type() == Type::Complex);
  assert(indexEvaluation->type() == Type::Matrix || indexEvaluation->type() == Type::Complex);
  if (baseEvaluation->type() == Type::Matrix || indexEvaluation->type() == Type::Matrix) {
    delete baseEvaluation;
    delete indexEvaluation;
    return new Complex(Complex::Float(NAN));
  }
  Expression * operands[2];
  operands[0] = baseEvaluation;
  Expression * operandChildren[2];
  operandChildren[0] = new Complex(Complex::Float(1.0f));
  operandChildren[1] = indexEvaluation;
  Expression * fraction = new Fraction(operandChildren, true);
  operands[1] = fraction->evaluate(context, angleUnit);
  Expression * power = new Power(operands, true);
  Expression * newResult = power->evaluate(context, angleUnit);
  delete baseEvaluation;
  delete operandChildren[0];
  delete indexEvaluation;
  delete fraction;
  delete operands[1];
  delete power;
  return newResult;
}

}
