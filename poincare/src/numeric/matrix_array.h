#ifndef POINCARE_NUMERIC_MATRIX_ARRAY_H
#define POINCARE_NUMERIC_MATRIX_ARRAY_H

namespace Poincare::Internal {

class MatrixArray {
 public:
  /* Compute M * N in result */
  template <typename T>
  static void Multiplication(T* m, T* n, T* result, int mNumberOfColumns,
                             int mNumberOfRows, int nNumberOfColumns);

  /* Inverse the array in-place. Array has to be given in the form
   * array[row_index][column_index] */
  template <typename T>
  static int Inverse(T* array, int numberOfRows, int numberOfColumns);

 private:
  /* Cap the matrix's size for inverse and determinant computation.
   * TODO: Find another solution */
  constexpr static int k_maxNumberOfChildren = 100;

  // Row canonize the array in place
  template <typename T>
  static void RowCanonize(T* array, int numberOfRows, int numberOfColumns,
                          T* c = nullptr, bool reduced = true);
};

}  // namespace Poincare::Internal

#endif
