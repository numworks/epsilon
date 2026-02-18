#include "approximation.h"
#include "matrix.h"

namespace Poincare::Internal::Approximation::Private {

template <typename T>
std::complex<T> Approximator::approximateTrace(const Tree* e) {
  Tree* matrix = toMatrix(e);
  int n = Matrix::NumberOfRows(matrix);
  assert(n == Matrix::NumberOfColumns(matrix));
  std::complex<T> result = std::complex<T>(0);
  const Tree* child = matrix->child(0);
  for (int i = 0; i < n - 1; i++) {
    result += privateToComplexRecursive<T>(child);
    if (std::isnan(result.real()) || std::isnan(result.imag())) {
      matrix->removeTree();
      return std::complex<T>(NAN, NAN);
    }
    for (int j = 0; j < n + 1; j++) {
      child = child->nextTree();
    }
  }
  result += privateToComplexRecursive<T>(child);
  matrix->removeTree();
  return result;
}

#if !POINCARE_NO_FLOAT_APPROXIMATION
template std::complex<float> Approximator::approximateTrace<float>(
    const Tree* matrix);
#endif
template std::complex<double> Approximator::approximateTrace<double>(
    const Tree* matrix);

}  // namespace Poincare::Internal::Approximation::Private
