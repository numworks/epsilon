#ifndef POINCARE_MATRIX_COMPLEX_H
#define POINCARE_MATRIX_COMPLEX_H

#include <poincare/evaluation.h>
#include <poincare/allocation_failure_evaluation_node.h>
#include <poincare/complex.h>

namespace Poincare {

template<typename T>
class MatrixComplex;

template<typename T>
class MatrixComplexNode : public EvaluationNode<T> {
public:
  static MatrixComplexNode<T> * FailedAllocationStaticNode();
  MatrixComplexNode<T> * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  MatrixComplexNode() :
    EvaluationNode<T>(),
    m_numberOfRows(0),
    m_numberOfColumns(0)
  {}

  ComplexNode<T> * complexAtIndex(int index);

  // TreeNode
  size_t size() const override { return sizeof(MatrixComplexNode<T>); }
#if TREE_LOG
  const char * description() const override { return "Evaluation matrix complex";  }
#endif
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }

  // EvaluationNode
  typename EvaluationNode<T>::Type type() const override { return EvaluationNode<T>::Type::MatrixComplex; }
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  virtual void setNumberOfRows(int rows) { assert(rows >= 0); m_numberOfRows = rows; }
  virtual void setNumberOfColumns(int columns) { assert(columns >= 0); m_numberOfColumns = columns; }
  bool isUndefined() const override;
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override;
  std::complex<T> determinant() const override;
  Evaluation<T> inverse() const override;
  Evaluation<T> transpose() const override;
private:
  int m_numberOfRows;
  int m_numberOfColumns;
};

template<typename T>
class AllocationFailureMatrixComplexNode : public AllocationFailureEvaluationNode<MatrixComplexNode, T> {
  void setNumberOfRows(int rows) override {}
  void setNumberOfColumns(int columns) override {}
};

template<typename T>
class MatrixComplex : public Evaluation<T> {
  friend class MatrixComplexNode<T>;
public:
  MatrixComplex(MatrixComplexNode<T> * node) : Evaluation<T>(node) {}
  MatrixComplex() : Evaluation<T>(TreePool::sharedPool()->createTreeNode<MatrixComplexNode<T> >()) {}
  static MatrixComplex<T> Undefined() {
    std::complex<T> undef = std::complex<T>(NAN, NAN);
    return MatrixComplex<T>((std::complex<T> *)&undef, 1, 1);
  }
  static MatrixComplex<T> createIdentity(int dim);
  Complex<T> complexAtIndex(int index) {
    return Complex<T>(node()->complexAtIndex(index));
  }
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  void setDimensions(int rows, int columns);
  void addChildAtIndexInPlace(Complex<T> t, int index, int currentNumberOfChildren);
private:
  MatrixComplexNode<T> * node() { return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node()); }
  void setNumberOfRows(int rows) {
    assert(rows >= 0);
    node()->setNumberOfRows(rows);
  }
  void setNumberOfColumns(int columns) {
    assert(columns >= 0);
    node()->setNumberOfColumns(columns);
  }
  MatrixComplex(std::complex<T> * operands, int numberOfRows, int numberOfColumns);
  MatrixComplexNode<T> * node() const { return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node()); }
};

}

#endif
