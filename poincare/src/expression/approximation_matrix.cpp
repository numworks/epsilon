#include "approximation.h"
#include "matrix.h"

namespace Poincare::Internal::Approximation::Private {

template <typename T>
std::complex<T> ApproximateTrace(const Tree* matrix, const Context* ctx) {
  int n = Matrix::NumberOfRows(matrix);
  assert(n == Matrix::NumberOfColumns(matrix));
  std::complex<T> result = std::complex<T>(0);
  const Tree* child = matrix->child(0);
  for (int i = 0; i < n - 1; i++) {
    result += PrivateToComplex<T>(child, ctx);
    if (std::isnan(result.real()) || std::isnan(result.imag())) {
      return std::complex<T>(NAN, NAN);
    }
    for (int j = 0; j < n + 1; j++) {
      child = child->nextTree();
    }
  }
  result += PrivateToComplex<T>(child, ctx);
  return result;
}

template std::complex<float> ApproximateTrace<float>(const Tree* matrix,
                                                     const Context* ctx);
template std::complex<double> ApproximateTrace<double>(const Tree* matrix,
                                                       const Context* ctx);

}  // namespace Poincare::Internal::Approximation::Private
