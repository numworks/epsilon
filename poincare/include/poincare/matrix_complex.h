#ifndef POINCARE_MATRIX_COMPLEX_H
#define POINCARE_MATRIX_COMPLEX_H

#include <poincare/evaluation.h>
#include <poincare/complex.h>

namespace Poincare {

template<typename T>
class MatrixComplex;

template<typename T>
class MatrixComplexNode final : public EvaluationNode<T> {
public:
  MatrixComplexNode() :
    EvaluationNode<T>(),
    m_numberOfRows(0),
    m_numberOfColumns(0)
  {}

  std::complex<T> complexAtIndex(int index) const;

  // TreeNode
  size_t size() const override { return sizeof(MatrixComplexNode<T>); }
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }
  void didAddChildAtIndex(int newNumberOfChildren) override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixComplex";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " rows=\"" << m_numberOfRows << "\"";
    stream << " columns=\"" << m_numberOfColumns << "\"";
  }
#endif

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
  MatrixComplex<T> inverse() const;
  MatrixComplex<T> transpose() const;
private:
  // See comment on Matrix
  uint16_t m_numberOfRows;
  uint16_t m_numberOfColumns;
};

template<typename T>
class MatrixComplex final : public Evaluation<T> {
  friend class MatrixComplexNode<T>;
public:
  MatrixComplex(MatrixComplexNode<T> * node) : Evaluation<T>(node) {}
  static MatrixComplex Builder() { return TreeHandle::NAryBuilder<MatrixComplex<T>, MatrixComplexNode<T>>(); }
  static MatrixComplex Builder(std::complex<T> * operands, int numberOfRows, int numberOfColumns);
  static MatrixComplex<T> Undefined();
  static MatrixComplex<T> CreateIdentity(int dim);
  MatrixComplex<T> inverse() const { return node()->inverse(); }
  MatrixComplex<T> transpose() const { return node()->transpose(); }
  std::complex<T> complexAtIndex(int index) const {
    return node()->complexAtIndex(index);
  }
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  void setDimensions(int rows, int columns);
  void addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren);
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
  MatrixComplexNode<T> * node() const { return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node()); }
};

}

#endif
