#ifndef POINCARE_EXPRESSION_MATRIX_H
#define POINCARE_EXPRESSION_MATRIX_H

#include <poincare/src/memory/tree.h>

#include "approximation.h"
#include "dimension.h"

namespace Poincare::Internal {

struct Matrix {
  static uint8_t NumberOfRows(const Tree* matrix) {
    return matrix->nodeValue(0);
  }
  static uint8_t NumberOfColumns(const Tree* matrix) {
    return matrix->nodeValue(1);
  }
  static void SetNumberOfRows(Tree* matrix, uint8_t rows) {
    matrix->setNodeValue(0, rows);
  }
  static void SetNumberOfColumns(Tree* matrix, uint8_t cols) {
    matrix->setNodeValue(1, cols);
  }
  static void SetDimensions(Tree* matrix, uint8_t rows, uint8_t cols) {
    SetNumberOfRows(matrix, rows);
    SetNumberOfColumns(matrix, cols);
  }
  static Tree* Child(Tree* matrix, uint8_t row, uint8_t col) {
    assert(col < NumberOfColumns(matrix));
    return matrix->child(row * NumberOfColumns(matrix) + col);
  }
  static const Tree* Child(const Tree* matrix, uint8_t row, uint8_t col) {
    assert(col < NumberOfColumns(matrix));
    return matrix->child(row * NumberOfColumns(matrix) + col);
  }
  static Tree* Zero(MatrixDimension d);
  static Tree* Undef(MatrixDimension d);
  static Tree* Identity(const Tree* n);
  static Tree* Trace(const Tree* matrix);
  static Tree* Addition(const Tree* matrix1, const Tree* matrix2,
                        bool approximate = false,
                        const Approximation::Context* ctx = nullptr);
  static Tree* ScalarMultiplication(
      const Tree* scalar, const Tree* matrix, bool approximate = false,
      const Approximation::Context* ctx = nullptr);
  static Tree* Multiplication(const Tree* matrix1, const Tree* matrix2,
                              bool approximate = false,
                              const Approximation::Context* ctx = nullptr);
  static Tree* Transpose(const Tree* matrix);
  static bool RowCanonize(Tree* matrix, bool reducedForm = true,
                          Tree** determinant = nullptr,
                          bool approximate = false,
                          const Approximation::Context* ctx = nullptr,
                          bool forceCanonization = false);
  static int Rank(const Tree* matrix);
  static int CanonizeAndRank(Tree* matrix, bool forceCanonization = false);
  static int RankOfCanonized(const Tree* matrix);
  static Tree* Inverse(const Tree* matrix, bool approximate = false,
                       const Approximation::Context* ctx = nullptr);
  static Tree* Power(const Tree* matrix, int exponent, bool approximate = false,
                     const Approximation::Context* ctx = nullptr);
  static bool SystematicReduceMatrixOperation(Tree* e);
};

}  // namespace Poincare::Internal

#endif
