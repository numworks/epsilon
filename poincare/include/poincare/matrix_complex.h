#ifndef POINCARE_MATRIX_COMPLEX_H
#define POINCARE_MATRIX_COMPLEX_H

#include <poincare/evaluation.h>
#include <poincare/complex.h>

namespace Poincare {

template<typename T>
class MatrixComplex;

template<typename T>
class MatrixComplexNode : public EvaluationNode<T> {
public:
  MatrixComplexNode() :
    EvaluationNode<T>(),
    m_numberOfRows(0),
    m_numberOfColumns(0)
  {}

  // TreeNode
  size_t size() const override { return sizeof(MatrixComplexNode<T>); }
#if TREE_LOG
  const char * description() const override { return "Evaluation matrix complex";  }
#endif
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }

  typename Poincare::EvaluationNode<T>::Type type() const override { return Poincare::EvaluationNode<T>::Type::MatrixComplex; }
  int numberOfComplexOperands() const { return m_numberOfRows*m_numberOfColumns; }
  // WARNING: complexOperand may return a nullptr
  ComplexNode<T> * childAtIndex(int index) const override;
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  void setNumberOfRows(int rows) { assert(rows >= 0); m_numberOfRows = rows; }
  void setNumberOfColumns(int columns) { assert(columns >= 0); m_numberOfColumns = columns; }
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
class MatrixComplex : public Evaluation<T> {
friend class MatrixComplexNode<T>;
public:
  //MatrixComplex(TreeNode * t) : Evaluation<T>(t) {}
  MatrixComplex() : Evaluation<T>(TreePool::sharedPool()->createTreeNode<MatrixComplexNode<T> >(), true) {}
  static MatrixComplex<T> Undefined() {
    std::complex<T> undef = std::complex<T>(NAN, NAN);
    return MatrixComplex<T>((std::complex<T> *)&undef, 1, 1);
  }
  static MatrixComplex<T> createIdentity(int dim);
  int numberOfRows() const;
  int numberOfColumns() const;
  void setDimensions(int rows, int columns);
  void addChildAtIndex(Complex<T> t, int index, int currentNumberOfChildren) override;
private:
  void setNumberOfRows(int rows);
  void setNumberOfColumns(int columns);
  MatrixComplex(std::complex<T> * operands, int numberOfRows, int numberOfColumns);
  MatrixComplexNode<T> * node() const { return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node()); }
};

}

#endif
