#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/array.h>
#include <poincare/expression.h>

namespace Poincare {

class MatrixNode final : public Array, public ExpressionNode {
 public:
  MatrixNode() : Array() {}

  // TreeNode
  size_t size() const override { return sizeof(MatrixNode); }
  int numberOfChildren() const override {
    return m_numberOfRows * m_numberOfColumns;
  }
  void didChangeArity(int newNumberOfChildren) override {
    return Array::didChangeNumberOfChildren(newNumberOfChildren);
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Matrix"; }
  void logAttributes(std::ostream& stream) const override {
    stream << " rows=\"" << m_numberOfRows << "\"";
    stream << " columns=\"" << m_numberOfColumns << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Matrix; }
  int polynomialDegree(Context* context, const char* symbolName) const override;

  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  // Approximation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode =
                    Preferences::PrintFloatMode::Decimal,
                int numberOfSignificantDigits = 0) const override;

 private:
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class Matrix final : public Expression {
  template <typename T>
  friend class MatrixComplexNode;
  friend class GlobalContext;

 public:
  /* Cap the matrix's size for inverse and determinant computation.
   * TODO: Find another solution */
  constexpr static int k_maxNumberOfChildren = 100;

  Matrix(const MatrixNode* node) : Expression(node) {}
  static Matrix Builder() {
    return TreeHandle::NAryBuilder<Matrix, MatrixNode>();
  }

  void setDimensions(int rows, int columns);
  Array::VectorType vectorType() const { return node()->vectorType(); }
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  using TreeHandle::addChildAtIndexInPlace;
  using TreeHandle::removeChildAtIndexInPlace;
  void addChildrenAsRowInPlace(TreeHandle t, int i);
  Expression matrixChild(int i, int j) {
    return childAtIndex(i * numberOfColumns() + j);
  }

  /* Operation on matrix */

  // rank returns -1 if the rank cannot be computed
  int rank(Context* context, Preferences::ComplexFormat complexFormat,
           Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
           ReductionTarget reductionTarget, bool inPlace = false);
  Expression createTrace();
  // Inverse the array in-place. Array has to be given in the form
  // array[row_index][column_index]
  template <typename T>
  static int ArrayInverse(T* array, int numberOfRows, int numberOfColumns);
  static Matrix CreateIdentity(int dim);
  Matrix createTranspose() const;
  Expression createRef(const ReductionContext& reductionContext,
                       bool* couldComputeRef, bool reduced) const;
  /* createInverse can be called on any matrix, reduced or not, approximated or
   * not. */
  Expression createInverse(const ReductionContext& reductionContext,
                           bool* couldComputeInverse) const;
  Expression determinant(const ReductionContext& reductionContext,
                         bool* couldComputeDeterminant, bool inPlace);
  Expression norm(const ReductionContext& reductionContext) const;
  Expression dot(Matrix* b, const ReductionContext& reductionContext) const;
  Matrix cross(Matrix* b, const ReductionContext& reductionContext) const;

  // Expression
  Expression shallowReduce(ReductionContext reductionContext);

 private:
  MatrixNode* node() const {
    return static_cast<MatrixNode*>(Expression::node());
  }
  void setNumberOfRows(int rows) { node()->setNumberOfRows(rows); }
  void setNumberOfColumns(int columns) { node()->setNumberOfColumns(columns); }
  Expression computeInverseOrDeterminant(
      bool computeDeterminant, const ReductionContext& reductionContext,
      bool* couldCompute) const;
  // rowCanonize turns a matrix in its row echelon form, reduced or not.
  Matrix rowCanonize(const ReductionContext& reductionContext,
                     Expression* determinant, bool reduced = true);
  // Row canonize the array in place
  template <typename T>
  static void ArrayRowCanonize(T* array, int numberOfRows, int numberOfColumns,
                               T* c = nullptr, bool reduced = true);
};

}  // namespace Poincare

#endif
