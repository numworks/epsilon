#ifndef POINCARE_MATRIX_COMPLEX_H
#define POINCARE_MATRIX_COMPLEX_H

#include "array.h"
#include "complex.h"
#include "evaluation.h"

namespace Poincare {

template <typename T>
class MatrixComplex;

template <typename T>
class MatrixComplexNode final : public Array, public EvaluationNode<T> {
 public:
  MatrixComplexNode() : Array(), EvaluationNode<T>() {}

  std::complex<T> complexAtIndex(int index) const override;

  // PoolObject
  size_t size() const override { return sizeof(MatrixComplexNode<T>); }
  int numberOfChildren() const override {
    return isUndefined() ? 0 : m_numberOfRows * m_numberOfColumns;
  }
  void didChangeArity(int newNumberOfChildren) override {
    return Array::didChangeNumberOfChildren(newNumberOfChildren);
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override {
    stream << "MatrixComplex";
  }
  void logAttributes(std::ostream &stream) const override {
    stream << " rows=\"" << m_numberOfRows << "\"";
    stream << " columns=\"" << m_numberOfColumns << "\"";
  }
#endif

  // EvaluationNode
  typename EvaluationNode<T>::Type otype() const override {
    return EvaluationNode<T>::Type::MatrixComplex;
  }
  bool isUndefined() const override {
    return m_numberOfRows == static_cast<uint16_t>(-1);
  }
  OExpression complexToExpression(
      Preferences::Preferences::ComplexFormat complexFormat) const override;

  // OMatrix related functions
  std::complex<T> trace() const;
  std::complex<T> determinant() const;
  MatrixComplex<T> inverse() const;
  MatrixComplex<T> transpose() const;
  MatrixComplex<T> ref(bool reduced) const;
};

template <typename T>
class MatrixComplex final : public Evaluation<T> {
  friend class MatrixComplexNode<T>;

 public:
  MatrixComplex(MatrixComplexNode<T> *node) : Evaluation<T>(node) {}
  static MatrixComplex Builder() {
    return PoolHandle::NAryBuilder<MatrixComplex<T>, MatrixComplexNode<T>>();
  }
  static MatrixComplex Builder(std::complex<T> *operands, int numberOfRows,
                               int numberOfColumns);
  static MatrixComplex<T> Undefined();
  std::complex<T> trace() const { return node()->trace(); }
  std::complex<T> determinant() const { return node()->determinant(); }
  MatrixComplex<T> inverse() const { return node()->inverse(); }
  MatrixComplex<T> transpose() const { return node()->transpose(); }
  MatrixComplex<T> ref(bool reduced) const { return node()->ref(reduced); }
  std::complex<T> complexAtIndex(int index) const {
    return node()->complexAtIndex(index);
  }
  Array::VectorType vectorType() const { return node()->vectorType(); }
  bool isVector() const { return node()->isVector(); }
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
  void setDimensions(int rows, int columns);
  void addChildAtIndexInPlace(Evaluation<T> t, int index,
                              int currentNumberOfChildren);

 private:
  MatrixComplexNode<T> *node() const {
    return static_cast<MatrixComplexNode<T> *>(Evaluation<T>::node());
  }
  void setNumberOfRows(int rows) { node()->setNumberOfRows(rows); }
  void setNumberOfColumns(int columns) { node()->setNumberOfColumns(columns); }
};

}  // namespace Poincare

#endif
