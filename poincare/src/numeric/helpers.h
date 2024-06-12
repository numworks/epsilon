#ifndef POINCARE_NUMERIC_HELPERS_H
#define POINCARE_NUMERIC_HELPERS_H

namespace Poincare::Internal {

/* Compute M * N in result */
template <typename T>
void MatrixMultiplicationOnArrays(T* m, T* n, T* result, int mNumberOfColumns,
                                  int mNumberOfRows, int nNumberOfColumns);

}  // namespace Poincare::Internal

#endif
