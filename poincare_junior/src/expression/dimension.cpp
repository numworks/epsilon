#include "dimension.h"

#include "approximation.h"
#include "matrix.h"

namespace PoincareJ {

Dimension Dimension::ComputeDimension(const Tree* t) {
  Dimension childDim[t->numberOfChildren()];
  for (int i = 0; const Tree* child : t->children()) {
    Dimension dim = ComputeDimension(child);
    if (dim.isInvalid()) {
      return Invalid();
    }
    childDim[i++] = dim;
  }
  switch (t->type()) {
    case BlockType::Addition:
    case BlockType::Subtraction:
      assert(t->numberOfChildren() > 0);
      for (int i = 1; i < t->numberOfChildren(); i++) {
        if (childDim[0] != childDim[i]) {
          return Invalid();
        }
      }
      return childDim[0];
    case BlockType::Multiplication: {
      assert(t->numberOfChildren() > 0);
      Dimension current = childDim[0];
      for (int i = 1; i < t->numberOfChildren(); i++) {
        Dimension next = childDim[i];
        if (current.isMatrix() && next.isMatrix()) {
          if (current.matrix.cols != next.matrix.rows) {
            return Invalid();
          }
          current = Matrix(current.matrix.rows, next.matrix.cols);
        } else {
          current = next;
        }
      }
      return current;
    }
    case BlockType::Power:
      if (!childDim[1].isScalar() ||
          (childDim[0].isMatrix() && !childDim[0].isSquareMatrix())) {
        return Invalid();
      }
      return childDim[0];
    case BlockType::Matrix:
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (!childDim[i].isScalar()) {
          return Invalid();
        }
      }
      return Matrix(Matrix::NumberOfRows(t), Matrix::NumberOfColumns(t));
    case BlockType::Dim:
      return childDim[0].isMatrix() ? Matrix(1, 2) : Invalid();
    case BlockType::Det:
    case BlockType::Trace:
      return childDim[0].isSquareMatrix() ? Scalar() : Invalid();
    case BlockType::Inverse:
      return childDim[0].isSquareMatrix() ? childDim[0] : Invalid();
    case BlockType::Transpose:
      return childDim[0].isMatrix()
                 ? Matrix(childDim[0].matrix.cols, childDim[0].matrix.rows)
                 : Invalid();
    case BlockType::Ref:
    case BlockType::Rref:
      return childDim[0].isMatrix() ? childDim[0] : Invalid();
    case BlockType::Identity: {
      if (!childDim[0].isScalar() ||
          !t->childAtIndex(0)->block()->isInteger()) {
        // what should be done with unknown arg ?
        return Invalid();
      }
      int n = Approximation::To<float>(t->childAtIndex(0));
      return Matrix(n, n);
    }
    case BlockType::Norm:
      return childDim[0].isVector() ? Scalar() : Invalid();
    case BlockType::Dot:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) ? Scalar()
                                                                    : Invalid();
    case BlockType::Cross:
      return childDim[0].isVector() && (childDim[0] == childDim[1]) &&
                     (childDim[0].matrix.rows == 3 ||
                      childDim[0].matrix.cols == 3)
                 ? childDim[0]
                 : Invalid();
    default:
      // Scalar-only constants and functions
      for (int i = 0; i < t->numberOfChildren(); i++) {
        if (!childDim[i].isScalar()) {
          return Invalid();
        }
      }
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
