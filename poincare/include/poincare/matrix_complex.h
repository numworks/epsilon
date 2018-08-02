#ifndef POINCARE_MATRIX_COMPLEX_H
#define POINCARE_MATRIX_COMPLEX_H

#include <poincare/evaluation.h>
#include <poincare/complex.h>

namespace Poincare {

template<typename T>
class MatrixComplexReference;

template<typename T>
class MatrixComplexNode : public EvaluationNode<T> {
public:
  // TreeNode
  size_t size() const override { return sizeof(MatrixComplexNode<T>); }
  const char * description() const override { return "Evaluation matrix complex";  }
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }

  void setMatrixComplexDimension(int numberOfRows, int numberOfColumns);
  typename Poincare::EvaluationNode<T>::Type type() const override { return Poincare::EvaluationNode<T>::Type::MatrixComplex; }
  int numberOfComplexOperands() const { return m_numberOfRows*m_numberOfColumns; }
  // WARNING: complexOperand may return a nullptr
  ComplexNode<T> * childAtIndex(int index) const override;
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  bool isUndefined() const override;
  ExpressionReference complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override;
  std::complex<T> determinant() const override;
  EvaluationReference<T> inverse() const override;
  EvaluationReference<T> transpose() const override;
private:
  // TODO: find another solution for inverse and determinant (avoid capping the matrix)
  static constexpr int k_maxNumberOfCoefficients = 100;
  int m_numberOfRows;
  int m_numberOfColumns;
};

template<typename T>
class MatrixComplexReference : public EvaluationReference<T> {
public:
  MatrixComplexReference(TreeNode * t) : EvaluationReference<T>(t) {}
  MatrixComplexReference(int numberOfRows, int numberOfColumns);
  static MatrixComplexReference<T> Undefined() {
    std::complex<T> undef = std::complex<T>(NAN, NAN);
    return MatrixComplexReference<T>((std::complex<T> *)&undef, 1, 1);
  }
  static MatrixComplexReference<T> createIdentity(int dim);
  int numberOfRows() const;
  int numberOfColumns() const;
private:
  MatrixComplexReference(std::complex<T> * operands, int numberOfRows, int numberOfColumns);
  MatrixComplexNode<T> * node() const override{ return static_cast<MatrixComplexNode<T> *>(TreeReference::node()); }
};

}

#endif
