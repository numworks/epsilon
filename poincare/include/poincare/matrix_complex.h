#ifndef POINCARE_MATRIX_COMPLEX_H
#define POINCARE_MATRIX_COMPLEX_H

#include <poincare/array.h>
#include <poincare/complex.h>
#include <poincare/evaluation.h>

namespace Poincare {

template<typename T>
class MatrixComplex;

template<typename T>
class MatrixComplexNode final : public Array, public EvaluationNode<T> {
public:
  MatrixComplexNode() :
    Array(),
    EvaluationNode<T>()
  {}

  std::complex<T> complexAtIndex(int index) const;

  // TreeNode
  size_t size() const override { return sizeof(MatrixComplexNode<T>); }
  int numberOfChildren() const override { return m_numberOfRows*m_numberOfColumns; }
  void didChangeArity(int newNumberOfChildren) override { return Array::didChangeNumberOfChildren(newNumberOfChildren); }
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
  bool isUndefined() const override;
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override;
  std::complex<T> determinant() const override;
  MatrixComplex<T> inverse() const;
  MatrixComplex<T> transpose() const;
  MatrixComplex<T> ref(bool reduced) const;
  std::complex<T> norm() const override;
  std::complex<T> dot(Evaluation<T> * e) const override;
  Evaluation<T> cross(Evaluation<T> * e) const override;
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
  MatrixComplex<T> ref(bool reduced) const { return node()->ref(reduced); }
  std::complex<T> complexAtIndex(int index) const {
    return node()->complexAtIndex(index);
  }
  Array::VectorType vectorType() const { return node()->vectorType(); }
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  void setDimensions(int rows, int columns);
  void addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren);
private:
  MatrixComplexNode<T> * node() { return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node()); }
  void setNumberOfRows(int rows) { node()->setNumberOfRows(rows); }
  void setNumberOfColumns(int columns) { node()->setNumberOfColumns(columns); }
  MatrixComplexNode<T> * node() const { return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node()); }
};

}

#endif
