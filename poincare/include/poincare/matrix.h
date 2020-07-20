#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixNode /*final*/ : public ExpressionNode {
public:
  MatrixNode() :
    m_numberOfRows(0),
    m_numberOfColumns(0) {}

  bool hasMatrixChild(Context * context) const;
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  virtual void setNumberOfRows(int rows) { assert(rows >= 0); m_numberOfRows = rows; }
  virtual void setNumberOfColumns(int columns) { assert(columns >= 0); m_numberOfColumns = columns; }

  // TreeNode
  size_t size() const override { return sizeof(MatrixNode); }
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }
  void didAddChildAtIndex(int newNumberOfChildren) override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Matrix";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " rows=\"" << m_numberOfRows << "\"";
    stream << " columns=\"" << m_numberOfColumns << "\"";
  }
#endif

  // Properties
  Type type() const override { return Type::Matrix; }
  int polynomialDegree(Context * context, const char * symbolName) const override;

  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };
  Expression shallowReduce(ReductionContext reductionContext) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>(context, complexFormat, angleUnit);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>(context, complexFormat, angleUnit);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  /* We could store 2 uint8_t but multiplying m_numberOfRows and
   * m_numberOfColumns could then lead to overflow. As we are unlikely to use
   * greater matrix than 100*100, uint16_t is fine. */
  uint16_t m_numberOfRows;
  uint16_t m_numberOfColumns;
};

class Matrix final : public Expression {
  template<typename T> friend class MatrixComplexNode;
  friend class GlobalContext;
public:
  Matrix(const MatrixNode * node) : Expression(node) {}
  static Matrix Builder() { return TreeHandle::NAryBuilder<Matrix, MatrixNode>(); }

  void setDimensions(int rows, int columns);
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  using TreeHandle::addChildAtIndexInPlace;
  void addChildrenAsRowInPlace(TreeHandle t, int i);
  Expression matrixChild(int i, int j) { return childAtIndex(i*numberOfColumns()+j); }

  /* Operation on matrix */
  int rank(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool inPlace = false);
  // Inverse the array in-place. Array has to be given in the form array[row_index][column_index]
  template<typename T> static int ArrayInverse(T * array, int numberOfRows, int numberOfColumns);
  static Matrix CreateIdentity(int dim);
  Matrix createTranspose() const;
  /* createInverse can be called on any matrix, reduced or not, approximated or
   * not. */
  Expression createInverse(ExpressionNode::ReductionContext reductionContext, bool * couldComputeInverse) const;
  Expression determinant(ExpressionNode::ReductionContext reductionContext, bool * couldComputeDeterminant, bool inPlace);
  // TODO: find another solution for inverse and determinant (avoid capping the matrix)
  static constexpr int k_maxNumberOfCoefficients = 100;

  // Expression
  Expression shallowReduce(Context * context);

private:
  MatrixNode * node() const { return static_cast<MatrixNode *>(Expression::node()); }
  void setNumberOfRows(int rows) { assert(rows >= 0); node()->setNumberOfRows(rows); }
  void setNumberOfColumns(int columns) { assert(columns >= 0); node()->setNumberOfColumns(columns); }
  Expression computeInverseOrDeterminant(bool computeDeterminant, ExpressionNode::ReductionContext reductionContext, bool * couldCompute) const;
  // rowCanonize turns a matrix in its reduced row echelon form.
  Matrix rowCanonize(ExpressionNode::ReductionContext reductionContext, Expression * determinant);
  // Row canonize the array in place
  template<typename T> static void ArrayRowCanonize(T * array, int numberOfRows, int numberOfColumns, T * c = nullptr);

};

}

#endif
