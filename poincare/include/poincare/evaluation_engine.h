#ifndef POINCARE_EVALUATION_ENGINE_H
#define POINCARE_EVALUATION_ENGINE_H

#include <poincare/expression.h>
#include <poincare/complex.h>

namespace Poincare {

class EvaluationEngine {
public:
  template <typename T> using ComplexCompute = Complex<T> (*)(const Complex<T>, Expression::AngleUnit angleUnit);
  template<typename T> static Complex<T> * approximate(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const Complex<T>, const Complex<T>);
  template<typename T> static Complex<T> * mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes);
};

}

#endif
