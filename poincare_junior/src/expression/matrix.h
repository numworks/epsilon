#ifndef POINCARE_EXPRESSION_MATRIX_H
#define POINCARE_EXPRESSION_MATRIX_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

struct Matrix {
  static uint8_t NumberOfRows(const Tree* matrix) {
    return static_cast<uint8_t>(matrix->block(1));
  }
  static uint8_t NumberOfColumns(const Tree* matrix) {
    return static_cast<uint8_t>(matrix->block(2));
  }
  static uint8_t SetNumberOfRows(Tree* matrix, uint8_t rows) {
    Block* nbRows = matrix->block() + 1;
    *(nbRows) = rows;
  }
  static uint8_t SetNumberOfColumns(Tree* matrix, uint8_t cols) {
    Block* nbCols = matrix->block() + 2;
    *(nbCols) = cols;
  }
  static Tree* ChildAtIndex(Tree* matrix, uint8_t row, uint8_t col) {
    return matrix->childAtIndex(row * NumberOfColumns(matrix) + col);
  }
  static const Tree* ChildAtIndex(const Tree* matrix, uint8_t row,
                                  uint8_t col) {
    assert(row < NumberOfRows(matrix));
    assert(col < NumberOfColumns(matrix));
    return matrix->childAtIndex(row * NumberOfColumns(matrix) + col);
  }
  static Tree* Identity(const Tree* n);
  static Tree* Trace(const Tree* matrix);
  static Tree* Addition(const Tree* a, const Tree* b);
  static Tree* ScalarMultiplication(const Tree* s, const Tree* m);
  static Tree* Multiplication(const Tree* a, const Tree* b);
  static Tree* Transpose(const Tree* matrix);
};

}  // namespace PoincareJ

#endif
