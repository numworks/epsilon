#ifndef POINCARE_DISTRIBUTION_METHOD_H
#define POINCARE_DISTRIBUTION_METHOD_H

#include <poincare/distribution.h>
#include <poincare/expression.h>

namespace Poincare {

class DistributionMethod {
 public:
  enum class Type : uint8_t {
    PDF,
    CDF,
    CDFRange,
    Inverse,
  };

  constexpr static int k_maxNumberOfParameters = 2;
  constexpr static int numberOfParameters(Type f) {
    switch (f) {
      case Type::PDF:
      case Type::CDF:
      case Type::Inverse:
        return 1;
      default:
        assert(f == Type::CDFRange);
        return 2;
    }
  }

  static const DistributionMethod* Get(Type type);

  virtual float EvaluateAtAbscissa(float* x, const Distribution* distribution,
                                   const float* parameters) const = 0;
  virtual double EvaluateAtAbscissa(double* x, const Distribution* distribution,
                                    const double* parameters) const = 0;

  virtual Expression shallowReduce(Expression* abscissae,
                                   const Distribution* distribution,
                                   Expression* parameters,
                                   ReductionContext context,
                                   Expression* expression) const {
    return *expression;
  }
};

}  // namespace Poincare

#endif
