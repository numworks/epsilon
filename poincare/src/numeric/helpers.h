#ifndef POINCARE_NUMERIC_HELPERS_H
#define POINCARE_NUMERIC_HELPERS_H

namespace Poincare::Internal {

/* Compute M * N in result */
template <typename T>
void MatrixMultiplicationOnArrays(T* m, T* n, T* result, int mNumberOfColumns,
                                  int mNumberOfRows, int nNumberOfColumns);

/* Inverse the array in-place. Array has to be given in the form
 * array[row_index][column_index] */
template <typename T>
int MatrixInverseOnArrays(T* array, int numberOfRows, int numberOfColumns);

}  // namespace Poincare::Internal

#endif
