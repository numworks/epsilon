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
    m_numberOfColumns = numberOfColumns;
  }
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  void setNumberOfRows(int rows) { assert(rows >= 0); m_numberOfRows = rows; }
  void setNumberOfColumns(int columns) { assert(columns >= 0); m_numberOfColumns = columns; }

  // TreeNode
  size_t size() const override { return sizeof(MatrixNode); }
#if TREE_LOG
  const char * description() const override { return "Matrix";  }
#endif
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }

  // Properties
  Type type() const override { return Type::Matrix; }
  int polynomialDegree(char symbolName) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<float>(context, angleUnit);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templatedApproximate<double>(context, angleUnit);
  }

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  int m_numberOfRows;
  int m_numberOfColumns;
};

class MatrixReference : public Expression {
  template<typename T> friend class MatrixComplexNode;
  friend class GlobalContext;
public:
  void setDimensions(int rows, int columns);
  int numberOfRows() const;
  int numberOfColumns() const;
  void addChildTreeAtIndex(TreeReference t, int index, int currentNumberOfChildren) override;
  Expression matrixChild(int i, int j) { return childAtIndex(i*numberOfColumns()+j); }

  /* Operation on matrix */
  int rank(Context & context, Preferences::AngleUnit angleUnit, bool inPlace);
  // Inverse the array in-place. Array has to be given in the form array[row_index][column_index]
  template<typename T> static int ArrayInverse(T * array, int numberOfRows, int numberOfColumns);
#if MATRIX_EXACT_REDUCING
  Expression trace() const;
  Expression determinant() const;
  MatrixReference transpose() const;
  static MatrixReference createIdentity(int dim);
  /* createInverse can be called on any matrix reduce or not, approximate or not. */
  Expression inverse(Context & context, Preferences::AngleUnit angleUnit) const;
#endif
private:
  // TODO: find another solution for inverse and determinant (avoid capping the matrix)
  static constexpr int k_maxNumberOfCoefficients = 100;

  MatrixReference(TreeNode * node) : Expression(node) {}
  MatrixNode * typedNode() const { assert(!isAllocationFailure()); return static_cast<MatrixNode *>(node()); }
  void setNumberOfRows(int rows);
  void setNumberOfColumns(int columns);
  /* rowCanonize turns a matrix in its reduced row echelon form. */
  void rowCanonize(Context & context, Preferences::AngleUnit angleUnit, Multiplication * m = nullptr);
  // Row canonize the array in place
  template<typename T> static void ArrayRowCanonize(T * array, int numberOfRows, int numberOfColumns, T * c = nullptr);
};

}

#endif
