#ifndef POINCARE_JUNIOR_DISTRIBUTION_METHOD_H
#define POINCARE_JUNIOR_DISTRIBUTION_METHOD_H

#include <poincare/expression.h>

#include "distribution.h"

namespace PoincareJ {

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

  static Type Get(const Tree* tree) {
    assert(tree->isDistribution());
    return tree->nodeValueBlock(2)->get<Type>();
  }

  static const DistributionMethod* Get(Type type);

  virtual float EvaluateAtAbscissa(float* x, const Distribution* distribution,
                                   const float* parameters) const = 0;
  virtual double EvaluateAtAbscissa(double* x, const Distribution* distribution,
                                    const double* parameters) const = 0;

  virtual bool shallowReduce(const Tree** abscissae,
                             const Distribution* distribution,
                             const Tree** parameters, Tree* expression) const {
    return false;
  }
};

}  // namespace PoincareJ

#endif
