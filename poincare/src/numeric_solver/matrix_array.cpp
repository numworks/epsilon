#include "matrix_array.h"

#include <omg/float.h>

namespace Poincare::Internal {

template <typename T>
void MatrixArray::Multiplication(T* m, T* n, T* result, int mNumberOfColumns,
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

template <typename T>
void MatrixArray::RowCanonize(T* array, int numberOfRows, int numberOfColumns,
                              T* determinant, bool reduced) {
  int h = 0;  // row pivot
  int k = 0;  // column pivot

  bool undef = false;
  for (int row = 0; row < numberOfRows; row++) {
    if (undef) {
      break;
    }
    for (int col = 0; col < numberOfColumns; col++) {
      if (std::isnan(std::abs(array[row * numberOfColumns + col]))) {
        undef = true;
        break;
      }
    }
  }
  if (undef) {
    for (int row = 0; row < numberOfRows; row++) {
      for (int col = 0; col < numberOfColumns; col++) {
        array[row * numberOfColumns + col] = static_cast<T>(NAN);
      }
    }
    if (determinant) {
      *determinant = static_cast<T>(NAN);
    }
  }

  while (h < numberOfRows && k < numberOfColumns) {
    // Find the biggest pivot (in absolute value). See comment on rowCanonize.
    int iPivot_temp = h;
    int iPivot = h;
    // Using double to stay accurate with any type T
    double bestPivot = 0.0;
    while (iPivot_temp < numberOfRows) {
      double pivot = std::abs(array[iPivot_temp * numberOfColumns + k]);
      if (pivot > bestPivot) {
        // Update best pivot
        bestPivot = pivot;
        iPivot = iPivot_temp;
        if (reduced) {
          /* In reduced form, taking the first non null pivot is enough, and
           * more efficient. */
          break;
        }
      }
      iPivot_temp++;
    }
    if (bestPivot < DBL_MIN) {
      // No non-null coefficient in this column, skip
      k++;
      // Update determinant: det *= 0
      if (determinant) {
        *determinant *= static_cast<T>(0.0);
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < numberOfColumns; col++) {
          // Swap array[iPivot, col] and array[h, col]
          T temp = array[iPivot * numberOfColumns + col];
          array[iPivot * numberOfColumns + col] =
              array[h * numberOfColumns + col];
          array[h * numberOfColumns + col] = temp;
        }
        // Update determinant: det *= -1
        if (determinant) {
          *determinant *= static_cast<T>(-1.0);
        }
      }
      // Set to 1 array[h][k] by linear combination
      T divisor = array[h * numberOfColumns + k];
      // Update determinant: det *= divisor
      if (determinant) {
        *determinant *= divisor;
      }
      for (int j = k + 1; j < numberOfColumns; j++) {
        array[h * numberOfColumns + j] /= divisor;
      }
      array[h * numberOfColumns + k] = 1;

      int l = reduced ? 0 : h + 1;
      /* Set to 0 all M[i][j] i != h, j > k by linear combination. If a
       * non-reduced form is computed (ref), only rows below the pivot are
       * reduced, i > h as well */
      for (int i = l; i < numberOfRows; i++) {
        if (i == h) {
          continue;
        }
        T factor = array[i * numberOfColumns + k];
        for (int j = k + 1; j < numberOfColumns; j++) {
          array[i * numberOfColumns + j] -=
              array[h * numberOfColumns + j] * factor;
        }
        array[i * numberOfColumns + k] = 0;
      }
      h++;
      k++;
    }
  }
}

template <typename T>
int MatrixArray::Inverse(T* array, int numberOfRows, int numberOfColumns) {
  if (numberOfRows != numberOfColumns) {
    return -1;
  }
  assert(numberOfRows * numberOfColumns <= k_maxNumberOfChildren);
  int dim = numberOfRows;
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim
   * identity matrix (A is squared with less than k_maxNumberOfChildren so (A|I)
   * has less than 2*k_maxNumberOfChildren */
  T operands[2 * k_maxNumberOfChildren];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      T cell = array[i * numberOfColumns + j];
      // Using abs function to be compatible with both double and std::complex
      if (!std::isfinite(std::abs(cell))) {
        return -2;
      }
      operands[i * 2 * dim + j] = cell;
    }
    for (int j = dim; j < 2 * dim; j++) {
      operands[i * 2 * dim + j] = j - dim == i ? 1.0 : 0.0;
    }
  }
  RowCanonize(operands, dim, 2 * dim);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    T cell = operands[i * 2 * dim + i];
    if (!std::isfinite(std::abs(cell)) || std::abs(cell - static_cast<T>(1.0)) >
                                              OMG::Float::EpsilonLax<float>()) {
      return -2;
    }
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      array[i * numberOfColumns + j] = operands[i * 2 * dim + j + dim];
    }
  }
  return 0;
}

template void MatrixArray::Multiplication(double*, double*, double*, int, int,
                                          int);
template int MatrixArray::Inverse(double* array, int numberOfRows,
                                  int numberOfColumns);

}  // namespace Poincare::Internal
