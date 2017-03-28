#include <poincare/matrix_dimension.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

MatrixDimension::MatrixDimension() :
  Function("dimension")
{
}

Expression::Type MatrixDimension::type() const {
  return Type::MatrixDimension;
}

Expression * MatrixDimension::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  MatrixDimension * md = new MatrixDimension();
  md->setArgument(newOperands, numberOfOperands, cloneOperands);
  return md;
}

float MatrixDimension::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * MatrixDimension::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  if (evaluation->type() == Type::Complex) {
    delete evaluation;
    return new Complex(Complex::Float(NAN));
  }
  Expression * dimension = ((Matrix *)evaluation)->createDimensionMatrix(context, angleUnit);
  delete evaluation;
  return dimension;
}

}

