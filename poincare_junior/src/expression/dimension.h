#ifndef POINCARE_EXPRESSION_DIMENSION_H
#define POINCARE_EXPRESSION_DIMENSION_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

struct Dimension {
  enum class Type {
    Undef,
    Scalar,
    Matrix,
  };

  static Dimension Undef() { return {Type::Undef}; }
  static Dimension Scalar() { return {Type::Scalar}; }
  static Dimension Matrix(uint8_t rows, uint8_t cols) {
    return {.type = Type::Matrix, .matrix = {rows, cols}};
  }

  bool operator==(const Dimension& other) const;
  bool operator!=(const Dimension& other) const { return !(*this == other); };

  bool isUndef() const { return type == Type::Undef; }
  bool isScalar() const { return type == Type::Scalar; }
  bool isMatrix() const { return type == Type::Matrix; }
  bool isSquareMatrix() const {
    return type == Type::Matrix && matrix.rows == matrix.cols;
  }
  bool isVector() const {
    return type == Type::Matrix && (matrix.rows == 1 || matrix.cols == 1);
  }

  static Dimension ComputeDimension(const Tree* t);

  Type type;
  union {
    struct {
      uint8_t rows;
      uint8_t cols;
    } matrix;
    // TODO units and lists
  };
};

}  // namespace PoincareJ

#endif
