#include "approximation.h"
#include "matrix.h"

namespace Poincare::Internal::Approximation::Private {

template <typename T>
std::complex<T> ApproximateTrace(const Tree* e, Parameters params,
                                 const Context* ctx) {
  Tree* matrix = ToMatrix(e, Approximation::Precision<T>::value, params, ctx);
  int n = Matrix::NumberOfRows(matrix);
  assert(n == Matrix::NumberOfColumns(matrix));
  std::complex<T> result = std::complex<T>(0);
  const Tree* child = matrix->child(0);
  for (int i = 0; i < n - 1; i++) {
    result += PrivateToComplexRecursive<T>(child, params, ctx);
    if (std::isnan(result.real()) || std::isnan(result.imag())) {
      matrix->removeTree();
      return std::complex<T>(NAN, NAN);
    }
    for (int j = 0; j < n + 1; j++) {
      child = child->nextTree();
    }
  }
  result += PrivateToComplexRecursive<T>(child, params, ctx);
  matrix->removeTree();
  return result;
}

#if !POINCARE_NO_FLOAT_APPROXIMATION
template std::complex<float> ApproximateTrace<float>(const Tree* matrix,
                                                     Parameters,
                                                     const Context* ctx);
#endif
template std::complex<double> ApproximateTrace<double>(const Tree* matrix,
                                                       Parameters,
                                                       const Context* ctx);

}  // namespace Poincare::Internal::Approximation::Private
