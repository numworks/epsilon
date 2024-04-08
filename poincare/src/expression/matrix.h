#ifndef POINCARE_EXPRESSION_MATRIX_H
#define POINCARE_EXPRESSION_MATRIX_H

#include <poincare_junior/src/memory/tree.h>

#include "dimension.h"

namespace PoincareJ {

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
  static Tree* Child(Tree* matrix, uint8_t row, uint8_t col) {
    assert(col < NumberOfColumns(matrix));
    return matrix->child(row * NumberOfColumns(matrix) + col);
  }
  static const Tree* Child(const Tree* matrix, uint8_t row, uint8_t col) {
    assert(col < NumberOfColumns(matrix));
    return matrix->child(row * NumberOfColumns(matrix) + col);
  }
  static Tree* Zero(MatrixDimension d);
  static Tree* Identity(const Tree* n);
  static Tree* Trace(const Tree* matrix, bool approximate = false);
  static Tree* Addition(const Tree* a, const Tree* b, bool approximate = false);
  static Tree* ScalarMultiplication(const Tree* s, const Tree* m,
                                    bool approximate = false);
  static Tree* Multiplication(const Tree* a, const Tree* b,
                              bool approximate = false);
  static Tree* Transpose(const Tree* matrix);
  static bool RowCanonize(Tree* m, bool reduced = true,
                          Tree** determinant = nullptr,
                          bool approximate = false);
  static int Rank(const Tree* m);
  static int CanonizeAndRank(Tree* m);
  static int RankOfCanonized(const Tree* m);
  static Tree* Inverse(const Tree* m, bool approximate = false);
  static Tree* Power(const Tree* m, int p, bool approximate = false);
  static bool SimplifySwitch(Tree* u);
};

}  // namespace PoincareJ

#endif
