#ifndef POINCARE_MATRIX_H
#define POINCARE_MATRIX_H

#include <poincare/expression_reference.h>
#include <poincare/matrix_data.h>

namespace Poincare {

class Multiplication;

class MatrixNode : public ExpressionNode {
public:
  void setDimensions(int numberOfRows, int numberOfColumns) {
    m_numberOfRows = numberOfRows;
    m_numberOfColumns = m_numberOfColumns;
  }
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }

  // TreeNode
  size_t size() const override { return sizeof(MatrixNode); }
#if TREE_LOG
  const char * description() const override { return "Matrix";  }
#endif
  int numberOfChildren() const { return m_numberOfRows*m_numberOfColumns; }

  // Properties
  Type type() const override { return Type::Matrix; }
  int polynomialDegree(char symbolName) const override;

  // Approximation
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>();
  }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>();
  }

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> EvaluationReference<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class MatrixReference : public ExpressionReference {
template<typename T> friend class MatrixComplexNode;
friend class GlobalContext;
public:
  MatrixNode * typedNode() const { assert(!isAllocationFailure()); return static_cast<MatrixNode *>(node()); }
  int numberOfRows() const;
  int numberOfColumns() const;
  ExpressionReference matrixChild(int i, int j) { assert(!isAllocationFailure()); return childAtIndex(i*numberOfColumns()+j); }

  /* Operation on matrix */
  int rank(Context & context, Preferences::AngleUnit angleUnit, bool inPlace);
  // Inverse the array in-place. Array has to be given in the form array[row_index][column_index]
  template<typename T> static int ArrayInverse(T * array, int numberOfRows, int numberOfColumns);
#if MATRIX_EXACT_REDUCING
  ExpressionReference trace() const;
  ExpressionReference determinant() const;
  MatrixReference transpose() const;
  static MatrixReference createIdentity(int dim);
  /* createInverse can be called on any matrix reduce or not, approximate or not. */
  ExpressionReference inverse(Context & context, Preferences::AngleUnit angleUnit) const;
#endif
private:
  /* rowCanonize turns a matrix in its reduced row echelon form. */
  void rowCanonize(Context & context, Preferences::AngleUnit angleUnit, Multiplication * m = nullptr);
  // Row canonize the array in place
  template<typename T> static void ArrayRowCanonize(T * array, int numberOfRows, int numberOfColumns, T * c = nullptr);
};

}

#endif
