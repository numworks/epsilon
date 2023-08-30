#include "dimension.h"

#include "approximation.h"
#include "matrix.h"
#include "symbol.h"

namespace PoincareJ {

bool Dimension::DeepCheckDimensions(const Tree* t) {
  Dimension childDim[t->numberOfChildren()];
  for (int i = 0; const Tree* child : t->children()) {
    if (!DeepCheckDimensions(child)) {
      return false;
    }
    childDim[i++] = GetDimension(child);
  }
  switch (t->type()) {
    case BlockType::Addition:
    case BlockType::Subtraction:
      assert(t->numberOfChildren() > 0);
      for (int i = 1; i < t->numberOfChildren(); i++) {
        if (childDim[0] != childDim[i]) {
          return false;
        }
      }
      return true;
    case BlockType::Multiplication: {
      assert(t->numberOfChildren() > 0);
      uint8_t cols = 0;
      for (int i = 0; i < t->numberOfChildren(); i++) {
        Dimension next = childDim[i];
        if (next.isMatrix()) {
          if (cols && cols != next.matrix.rows) {
            return false;
          }
          cols = next.matrix.cols;
        }
      }
      return true;
    }
    case BlockType::Power:
    case BlockType::PowerMatrix:
      return childDim[1].isScalar() &&
             (!childDim[0].isMatrix() || childDim[0].isSquareMatrix());
    case BlockType::Dim:
    case BlockType::Ref:
    case BlockType::Rref:
    case BlockType::Transpose:
      return childDim[0].isMatrix();
    case BlockType::Det:
    case BlockType::Trace:
    case BlockType::Inverse:
      return childDim[0].isSquareMatrix();
    case BlockType::Identity:
      // TODO check for unknowns and display error message if not integral
      return childDim[0].isScalar() && t->childAtIndex(0)->block()->isInteger();
    case BlockType::Norm:
      return childDim[0].isVector();
    case BlockType::Dot:
      return childDim[0].isVector() && (childDim[0] == childDim[1]);
    case BlockType::Cross:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) &&
             (childDim[0].matrix.rows == 3 || childDim[0].matrix.cols == 3);
    default:
      assert(t->block()->isScalarOnly());
    case BlockType::Matrix:
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (!childDim[i].isScalar()) {
          return false;
        }
      }
      return true;
  }
}

Dimension Dimension::GetDimension(const Tree* t) {
  switch (t->type()) {
    case BlockType::Multiplication: {
      uint8_t rows = 0;
      uint8_t cols = 0;
      for (const Tree* child : t->children()) {
        Dimension dim = GetDimension(child);
        if (dim.isMatrix()) {
          if (rows == 0) {
            rows = dim.matrix.rows;
          }
          cols = dim.matrix.cols;
        }
      }
      return rows > 0 ? Matrix(rows, cols) : Scalar();
    }
    case BlockType::Addition:
    case BlockType::Subtraction:
    case BlockType::Cross:
    case BlockType::Inverse:
    case BlockType::Power:
    case BlockType::PowerMatrix:
    case BlockType::Ref:
    case BlockType::Rref:
      return GetDimension(t->nextNode());
    case BlockType::Matrix:
      return Matrix(Matrix::NumberOfRows(t), Matrix::NumberOfColumns(t));
    case BlockType::Dim:
      return Matrix(1, 2);
    case BlockType::Transpose: {
      Dimension dim = GetDimension(t->nextNode());
      return Matrix(dim.matrix.cols, dim.matrix.rows);
    }
    case BlockType::Identity: {
      int n = Approximation::To<float>(t->childAtIndex(0));
      return Matrix(n, n);
    }
    case BlockType::UserSymbol: {
      char s[2];
      Symbol::GetName(t, s, 2);
      if ('A' <= s[0] && s[0] <= 'Z') {
        // TODO query the actual symbol and assume no unknown matrices
        return Matrix(1, 1);
      }
      return Scalar();
    }
    default:
      return Scalar();
  }
}

bool Dimension::operator==(const Dimension& other) const {
  if (type != other.type) {
    return false;
  }
  if (type == Type::Matrix) {
    return matrix.rows == other.matrix.rows && matrix.cols == other.matrix.cols;
  }
  return true;
}

}  // namespace PoincareJ
