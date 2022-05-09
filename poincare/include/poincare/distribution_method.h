#ifndef POINCARE_DISTRIBUTION_METHOD_H
#define POINCARE_DISTRIBUTION_METHOD_H

#include <poincare/expression.h>
#include <poincare/distribution.h>

namespace Poincare {

class DistributionMethod {
public:
  enum class Type : uint8_t {
  PDF,
  CDF,
  CDFRange,
  Inverse,
  };

  static constexpr int k_maxNumberOfParameters = 2;
  static constexpr int numberOfParameters(Type f) {
    switch(f) {
    case Type::PDF:
    case Type::CDF:
    case Type::Inverse:
      return 1;
    case Type::CDFRange:
      return 2;
    }
  }

  static void Initialize(DistributionMethod * distribution, Type type);

  virtual float EvaluateAtAbscissa(float * x, Distribution * distribution, const float * parameters) = 0;
  virtual double EvaluateAtAbscissa(double * x, Distribution * distribution, const double * parameters) = 0;

  virtual Expression shallowReduce(Expression * x, Context * context, Expression * expression) { return *expression; }
};

}

#endif
