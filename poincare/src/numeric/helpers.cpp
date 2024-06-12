#include "helpers.h"

namespace Poincare::Internal {

template <typename T>
void MatrixMultiplicationOnArrays(T* m, T* n, T* result, int mNumberOfColumns,
                                  int mNumberOfRows, int nNumberOfColumns) {
  for (int i = 0; i < mNumberOfRows; i++) {
    for (int j = 0; j < nNumberOfColumns; j++) {
      T res = 0.0f;
      for (int k = 0; k < mNumberOfColumns; k++) {
        res += m[i * mNumberOfColumns + k] * n[k * nNumberOfColumns + j];
      }
      result[i * nNumberOfColumns + j] = res;
    }
  }
}

template void MatrixMultiplicationOnArrays(double*, double*, double*, int, int,
                                           int);

}  // namespace Poincare::Internal
