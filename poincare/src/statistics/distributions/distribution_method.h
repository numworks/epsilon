#ifndef POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_METHOD_H
#define POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_METHOD_H

#include <poincare/src/memory/tree.h>
#include <poincare/statistics/distribution.h>

namespace Poincare::Internal {

namespace DistributionMethod {
enum class Type : uint8_t {
  PDF,
  CDF,
  CDFRange,
  Inverse,
};

Type GetType(const Tree* tree);

constexpr static int k_maxNumberOfParameters = 2;
template <typename T>
using Abscissae = std::array<T, k_maxNumberOfParameters>;

constexpr static int NumberOfParameters(Type f) {
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

template <typename T>
T EvaluateAtAbscissa(Type method, const Abscissae<T> x,
                     Distribution::Type distribType,
                     const Distribution::ParametersArray<T> parameters);

bool ShallowReduce(Type method, const Abscissae<const Tree*> abscissae,
                   Distribution::Type distribType,
                   const Distribution::ParametersArray<const Tree*> parameters,
                   Tree* expression);

};  // namespace DistributionMethod

}  // namespace Poincare::Internal

#endif
