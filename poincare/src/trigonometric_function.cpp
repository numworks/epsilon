#include <poincare/trigonometric_function.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

TrigonometricFunction::TrigonometricFunction(const char * name) :
  Function(name)
{
}

float TrigonometricFunction::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float input = m_args[0]->approximate(context, angleUnit);
  if (angleUnit == AngleUnit::Degree) {
    input *= M_PI/180.0f;
  }
  float result = trigonometricApproximation(input);
  if (input !=  0.0f && fabsf(result/input) <= 1E-7f) {
    return 0.0f;
  }
  return result;
}

Expression * TrigonometricFunction::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Matrix) {
    delete evaluation;
    return new Complex(Complex::Float(NAN));
  }
  /* Float case */
  if (((Complex *)evaluation)->b() == 0) {
    delete evaluation;
    return Function::privateEvaluate(context, angleUnit);
  }
  /* Complex case */
  Expression * resultEvaluation = createComplexEvaluation(evaluation, context, angleUnit);
  delete evaluation;
  return resultEvaluation;
}

}
